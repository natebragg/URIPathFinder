#include "rfc_3986.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#define MAKE_LEN(field, end) \
    size_t len_##field(const URI *uri) { \
        return uri->field ? (end - uri->field) : 0; \
    }
#define OR(a, b) \
    (uri->a ? uri->a : b)

MAKE_LEN(scheme,   uri->colon_s                                          )
MAKE_LEN(userinfo, uri->atsymbol                                         )
MAKE_LEN(host,     OR(colon_p,   OR(path, OR(question, OR(pound, uri->end)))))
MAKE_LEN(port,     OR(              path, OR(question, OR(pound, uri->end))))
MAKE_LEN(path,     OR(                       question, OR(pound, uri->end)))
MAKE_LEN(query,    OR(                                    pound, uri->end))
MAKE_LEN(fragment,                                               uri->end)

#define MAKE_GETTER(field) \
    char *get_##field(const URI *uri, char *buf, size_t *len){ \
        size_t f_len = len_##field(uri); \
        if (uri->field == NULL || f_len >= *len) { \
            *len = f_len; \
            return NULL; \
        } \
        strncpy(buf, uri->field, f_len); \
        buf[f_len] = '\0'; \
        return buf; \
    }

MAKE_GETTER(scheme)
MAKE_GETTER(userinfo)
MAKE_GETTER(host)
MAKE_GETTER(port)
MAKE_GETTER(path)
MAKE_GETTER(query)
MAKE_GETTER(fragment)

static parser alpha_parser = NULL;
static parser digit_parser = NULL;

void set_alpha_parser(parser p) {
    alpha_parser = p;
}

void set_digit_parser(parser p) {
    digit_parser = p;
}

// For the parsers other than parse_URI, the protocol is as
// follows: If the parser does not match, return NULL and don't
// modify the input string *s. If the parser matches, return a
// pointer to the first character of the match, and advance the
// input string *s to the first non-matching character.  If
// matched, the length of the match is (*s) - match.

// Match a single character
static const char *parse_char(const char **s, char c) {
    const char *match = NULL;
    if(**s == c) {
        match = *s;
        *s = (*s) + 1;
    }
    return match;
}

#define MAKE_PARSE(name, val) \
    static const char *parse_##name(const char **s) { \
        return parse_char(s, val); \
    }

MAKE_PARSE(colon,      ':')
MAKE_PARSE(atsymbol,   '@')
MAKE_PARSE(percent,    '%')
MAKE_PARSE(dash,       '-')
MAKE_PARSE(dot,        '.')
MAKE_PARSE(underscore, '_')
MAKE_PARSE(tilde,      '~')
MAKE_PARSE(fwd_slash,  '/')
MAKE_PARSE(question,   '?')
MAKE_PARSE(pound,      '#')
MAKE_PARSE(lbracket,   '[')
MAKE_PARSE(rbracket,   ']')
MAKE_PARSE(exclamation,'!')
MAKE_PARSE(dollar,     '$')
MAKE_PARSE(ampersand,  '&')
MAKE_PARSE(singlequote,'\'')
MAKE_PARSE(lparens,    '(')
MAKE_PARSE(rparens,    ')')
MAKE_PARSE(star,       '*')
MAKE_PARSE(plus,       '+')
MAKE_PARSE(comma,      ',')
MAKE_PARSE(semicolon,  ';')
MAKE_PARSE(equal,      '=')

static const char *parse_alpha(const char **s) {
    if (alpha_parser != NULL) {
        return alpha_parser(s);
    }
    const char *match = NULL;
    char c = **s;
    if (((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z'))) {
        match = *s;
        *s = (*s) + 1;
    }
    return match;
}

static const char *parse_digit(const char **s) {
    if (digit_parser != NULL) {
        return digit_parser(s);
    }
    const char *match = NULL;
    char c = **s;
    if ((c >= '0') && (c <= '9')) {
        match = *s;
        *s = (*s) + 1;
    }
    return match;
}

static const char *parse_hexdig(const char **s) {
    const char *match = NULL;
    char c = **s;
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'A') && (c <= 'F')) ||
        ((c >= 'a') && (c <= 'f'))) {
        match = *s;
        *s = (*s) + 1;
    }
    return match;
}

// Match the parser p exactly n times.
static const char *parse_n(const char **s, unsigned int n, parser p) {
    const char *match = *s;
    unsigned int i = 0;
    for (i = 0; i < n; i++) {
        const char *v = p(s);
        if (v == NULL) {
            *s = match;
            return NULL;
        }
    }
    return match;
}

// Match the parser p at least n times.
static const char *parse_n_star(const char **s, unsigned int n, parser p) {
    const char *match = parse_n(s, n, p);
    while (p(s) != NULL);
    return match;
}

// Match the parser p at least n times.
static const char *parse_n_to_m(const char **s, unsigned int n, unsigned int m, parser p) {
    const char *match = parse_n(s, n, p);
    unsigned int i = n;
    for (i = n; i < m && p(s) != NULL; i++);
    return match;
}

// Match the first of n parsers that matches
static const char *parse_opt(const char **s, unsigned int n, ...) {
    const char *match = NULL;
    va_list ap;
    va_start(ap, n);
    while (n-- > 0) {
        const char *tmp = *s;
        parser p = va_arg(ap, parser);
        match = p(&tmp);
        if (match != NULL) {
            *s = tmp;
            break;
        }
    }
    va_end(ap);
    return match;
}

// Match all parsers in order
static const char *parse_cat(const char **s, unsigned int n, ...) {
    const char *match = NULL;
    va_list ap;
    va_start(ap, n);
    if (n-- > 0) {
        parser p = va_arg(ap, parser);
        match = p(s);
    }
    if (match != NULL) {
        while (n-- > 0) {
            parser p = va_arg(ap, parser);
            if (p(s) == NULL) {
                // failed, rewind
                *s = match;
                match = NULL;
                break;
            }
        }
    }
    va_end(ap);
    return match;
}

// pct-encoded = "%" HEXDIG HEXDIG
static const char *parse_pct_encoded(const char **s) {
    return parse_cat(s, 3, parse_percent, parse_hexdig, parse_hexdig);
}

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
static const char *parse_unreserved(const char **s) {
    return parse_opt(s, 6, parse_alpha, parse_digit, parse_dash,
                        parse_dot, parse_underscore, parse_tilde);
}

// gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
static const char *parse_gen_delims(const char **s) {
    return parse_opt(s, 7, parse_colon, parse_fwd_slash, parse_question,
                           parse_pound, parse_lbracket, parse_rbracket, parse_atsymbol);
}

//    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//                  / "*" / "+" / "," / ";" / "="
static const char *parse_sub_delims(const char **s) {
    return parse_opt(s, 11, parse_exclamation, parse_dollar, parse_ampersand,
                            parse_singlequote, parse_lparens, parse_rparens, parse_star,
                            parse_plus, parse_comma, parse_semicolon, parse_equal);
}

// reserved = gen-delims / sub-delims
static const char *parse_reserved(const char **s) {
    return parse_opt(s, 2, parse_gen_delims, parse_sub_delims);
}

// pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
static const char *parse_pchar(const char **s) {
    return parse_opt(s, 5, parse_unreserved, parse_pct_encoded,
                        parse_sub_delims, parse_colon, parse_atsymbol);
}

// scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
static const char *parse_scheme_char(const char **s) {
    return parse_opt(s, 5, parse_alpha, parse_digit,
                           parse_plus, parse_dash, parse_dot);
}
static const char *parse_scheme(const char **s) {
    const char *match = parse_alpha(s);
    if (match != NULL) {
        parse_n_star(s, 0, parse_scheme_char);
    }
    return match;
}

// userinfo  = *( unreserved / pct-encoded / sub-delims / ":" )
static const char *parse_userinfo_char(const char **s) {
    return parse_opt(s, 4, parse_unreserved, parse_pct_encoded,
                           parse_sub_delims, parse_colon);
}
static const char *parse_userinfo(const char **s) {
    return parse_n_star(s, 0, parse_userinfo_char);
}

// reg-name = *( unreserved / pct-encoded / sub-delims )
static const char *parse_reg_name_char(const char **s) {
    return parse_opt(s, 3, parse_unreserved, parse_pct_encoded,
                           parse_sub_delims);
}
static const char *parse_reg_name(const char **s) {
    return parse_n_star(s, 0, parse_reg_name_char);
}

// IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
static const char *parse_unreserved_or_sub_delims_or_colon(const char **s) {
    return parse_opt(s, 3, parse_unreserved, parse_sub_delims, parse_colon);
}
static const char *parse_IPvFuture(const char **s) {
    const char *match = parse_char(s, 'v');
    if (match == NULL ||
        parse_n_star(s, 1, parse_hexdig) == NULL ||
        parse_dot(s) == NULL ||
        parse_n_star(s, 1, parse_unreserved_or_sub_delims_or_colon) == NULL) {
        *s = match;
        match = NULL;
    }
    return match;
}

// dec-octet = DIGIT                 ; 0-9
//           / %x31-39 DIGIT         ; 10-99
//           / "1" 2DIGIT            ; 100-199
//           / "2" %x30-34 DIGIT     ; 200-249
//           / "25" %x30-35          ; 250-255
static const char *parse_dec_octet(const char **s) {
    const char *match = parse_digit(s);
    if (match != NULL) {
        if (*match != '0') { // else case 1
            const char *d2 = parse_digit(s);
            if (d2 != NULL) { // else case 1
                const char *d3 = parse_digit(s);
                if (d3 != NULL) { // else case 2
                    if (!(*match == '1' || // else case 3
                          *match == '2' && *d2 <= '4' || // else case 4
                          *match == '2' && *d2 == '5' && *d3 <= '5')) // else case 5
                    { *s = d3; } // oops, still case 2, rewind
                }
            }
        }
    }
    return match;
}

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
static const char *parse_IPv4address(const char **s) {
    return parse_cat(s, 7, parse_dec_octet, parse_dot, parse_dec_octet, parse_dot,
                           parse_dec_octet, parse_dot, parse_dec_octet);
}

// h16 = 1*4HEXDIG
static const char *parse_h16(const char **s) {
    return parse_n_to_m(s, 1, 4, parse_hexdig);
}

static const char *parse_h16_colon(const char **s) {
    const char *match = parse_cat(s, 2, parse_h16, parse_colon);
    return match;
}

static const char *parse_h16_colon_h16(const char **s) {
    return parse_cat(s, 3, parse_h16, parse_colon, parse_h16);
}

// ls32 = ( h16 ":" h16 ) / IPv4address
static const char *parse_ls32(const char **s) {
    return parse_opt(s, 2, parse_h16_colon_h16, parse_IPv4address);
}

// IPv6address =                            6( h16 ":" ) ls32
static const char *parse_IPv6address_case_1(const char **s) {
    const char *match = parse_n(s, 6, parse_h16_colon);
    if (match != NULL && parse_ls32(s) == NULL)  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             /                       "::" 5( h16 ":" ) ls32
static const char *parse_IPv6address_case_2(const char **s) {
    const char *match = parse_colon(s);
    if (match != NULL &&
        (parse_colon(s) == NULL ||
         parse_n(s, 5, parse_h16_colon) == NULL ||
         parse_ls32(s) == NULL))  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [               h16 ] "::" 4( h16 ":" ) ls32
static const char *parse_colon_h16(const char **s) {
    return parse_cat(s, 2, parse_colon, parse_h16);
}
static const char *parse_IPv6address_segment(const char **s, int m) {
    const char *match = parse_h16(s);
    if (match != NULL) {
        parse_n_to_m(s, 0, m, parse_colon_h16);
    }
    const char *colon = parse_colon(s);
    if (colon == NULL || parse_colon(s) == NULL)
    {
        *s = match;
        match = NULL;
    } else if (match == NULL) {
        match = colon;
    }
    return match;
}
static const char *parse_IPv6address_case_3(const char **s) {
    const char *match = parse_IPv6address_segment(s, 0);
    if (match != NULL &&
        (parse_n(s, 4, parse_h16_colon) == NULL ||
         parse_ls32(s) == NULL)) {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
static const char *parse_IPv6address_case_4(const char **s) {
    const char *match = parse_IPv6address_segment(s, 1);
    if (match != NULL &&
        (parse_n(s, 3, parse_h16_colon) == NULL ||
         parse_ls32(s) == NULL))  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
static const char *parse_IPv6address_case_5(const char **s) {
    const char *match = parse_IPv6address_segment(s, 2);
    if (match != NULL &&
        (parse_n(s, 2, parse_h16_colon) == NULL ||
         parse_ls32(s) == NULL))  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
static const char *parse_IPv6address_case_6(const char **s) {
    const char *match = parse_IPv6address_segment(s, 3);
    if (match != NULL &&
        (parse_n(s, 1, parse_h16_colon) == NULL ||
         parse_ls32(s) == NULL))  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *4( h16 ":" ) h16 ] "::"              ls32
static const char *parse_IPv6address_case_7(const char **s) {
    const char *match = parse_IPv6address_segment(s, 4);
    if (match != NULL && parse_ls32(s) == NULL)  {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *5( h16 ":" ) h16 ] "::"              h16
static const char *parse_IPv6address_case_8(const char **s) {
    const char *match = parse_IPv6address_segment(s, 5);
    if (match != NULL && parse_h16(s) == NULL) {
        *s = match;
        match = NULL;
    }
    return match;
}
//             / [ *6( h16 ":" ) h16 ] "::"
static const char *parse_IPv6address_case_9(const char **s) {
    return parse_IPv6address_segment(s, 6);
}
static const char *parse_IPv6address(const char **s) {
    return parse_opt(s, 9, parse_IPv6address_case_1, parse_IPv6address_case_2,
                           parse_IPv6address_case_3, parse_IPv6address_case_4,
                           parse_IPv6address_case_5, parse_IPv6address_case_6,
                           parse_IPv6address_case_7, parse_IPv6address_case_8,
                           parse_IPv6address_case_9);
}

// IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
static const char *parse_IPv6address_or_IPvFuture(const char **s) {
    return parse_opt(s, 2, parse_IPv6address, parse_IPvFuture);
}

static const char *parse_IP_literal(const char **s) {
    return parse_cat(s, 3, parse_lbracket, parse_IPv6address_or_IPvFuture, parse_rbracket);
}

// host = IP-literal / IPv4address / reg-name
static const char *parse_host(const char **s) {
    return parse_opt(s, 3, parse_IP_literal, parse_IPv4address, parse_reg_name);
}

// port = *DIGIT
static const char *parse_port(const char **s) {
    return parse_n_star(s, 0, parse_digit);
}

// segment = *pchar
static const char *parse_segment(const char **s) {
    return parse_n_star(s, 0, parse_pchar);
}

// segment-nz = 1*pchar
static const char *parse_segment_nz(const char **s) {
    return parse_n_star(s, 1, parse_pchar);
}

// path-abempty = *( "/" segment )
static const char *parse_slash_segment(const char **s) {
    const char *match = parse_fwd_slash(s);
    if (match != NULL) {
        // segment always succeeds
        parse_segment(s);
    }
    return match;
}
static const char *parse_path_abempty(const char **s) {
    return parse_n_star(s, 0, parse_slash_segment);
}

// path-rootless = segment-nz *( "/" segment )
static const char *parse_path_rootless(const char **s) {
    const char *match = parse_segment_nz(s);
    if (match != NULL) {
        parse_path_abempty(s);
    }
    return match;
}

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
// begins with "/" but not "//"
static const char *parse_path_absolute(const char **s) {
    const char *match = parse_fwd_slash(s);
    if (match != NULL) {
        parse_path_rootless(s);
    }
    return match;
}

// path-empty    = 0<pchar>                            ; zero characters
static const char *parse_path_empty(const char **s) {
    return *s;
}

// hier-part = "//" authority path-abempty
//           / path-absolute
//           / path-rootless
//           / path-empty
// authority = [ userinfo "@" ] host [ ":" port ]
static const char *parse_hier_part(const char **s, const char **slash, const char **userinfo, const char **atsymbol, const char **host, const char **colon, const char **port) {
    *slash     = NULL;
    *userinfo  = NULL;
    *atsymbol  = NULL;
    *host      = NULL;
    *colon     = NULL;
    *port      = NULL;
    const char *path = NULL;

    if (// "//" authority path-abempty
        ((*slash = parse_fwd_slash(s)) != NULL) &&
        ((parse_fwd_slash(s) != NULL) ||
         // back up if the second '/' is missing
         ((*slash = NULL), (*s = (*s) - 1), false))) {
        // userinfo can be empty so will always succeed
        *userinfo = parse_userinfo(s);
        // backup if the '@' is missing
        if ((*atsymbol = parse_atsymbol(s)) == NULL) {
            *s = *userinfo;
            *userinfo = NULL;
        }
        // host can be empty so will always succeed
        *host = parse_host(s);
        if ((*colon = parse_colon(s)) != NULL) {
            *port = parse_port(s);
        }
        // path can be empty so will always succeed
        path = parse_path_abempty(s);
    } else if (((path = parse_path_absolute(s)) == NULL) &&
               ((path = parse_path_rootless(s)) == NULL)) {
        // path can be empty so will always succeed
        path = parse_path_empty(s);
    }

    return path;
}

// query = *( pchar / "/" / "?" )
static const char *parse_query_char(const char **s) {
    return parse_opt(s, 3, parse_pchar, parse_fwd_slash, parse_question);
}

static const char *parse_query(const char **s) {
    return parse_n_star(s, 0, parse_query_char);
}

// fragment = *( pchar / "/" / "?" )
static const char *parse_fragment_char(const char **s) {
    return parse_opt(s, 3, parse_pchar, parse_fwd_slash, parse_question);
}

static const char *parse_fragment(const char **s) {
    return parse_n_star(s, 0, parse_fragment_char);
}

// URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
URI parse_URI(const char *uri) {
    const char **s = &uri;

    URI result_null = {
        .scheme = NULL,
        .colon_s = NULL,
        .slash = NULL,
        .userinfo = NULL,
        .atsymbol = NULL,
        .host = NULL,
        .colon_p = NULL,
        .port = NULL,
        .path = NULL,
        .question = NULL,
        .query = NULL,
        .pound = NULL,
        .fragment = NULL,
        .end = NULL,
    };
    URI result = result_null;

    if ((result.scheme  = (char*)parse_scheme(s)) == NULL ||
        (result.colon_s = (char*)parse_colon(s)) == NULL ||
        //path can be empty so will always succeed
        (result.path    = (char*)parse_hier_part(s, (const char**)&result.slash,
                                                    (const char**)&result.userinfo,
                                                    (const char**)&result.atsymbol,
                                                    (const char**)&result.host,
                                                    (const char**)&result.colon_p,
                                                    (const char**)&result.port)) == NULL ||
        // if ? was found but no query
        (((result.question = (char*)parse_question(s)) != NULL) &&
         ((result.query    = (char*)parse_query(s)) == NULL)) ||
        // if # but no fragment
        (((result.pound    = (char*)parse_pound(s)) != NULL) &&
         ((result.fragment = (char*)parse_fragment(s)) == NULL)) ||
        (*(result.end      = (char*)*s) != '\0')) {
        result = result_null;
    }
    return result;
}
