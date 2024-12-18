// URIPathFinder: A simple parser for URIs
//
// BSD 3-Clause License
//
// Copyright (c) 2024, Nate Bragg
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "rfc_3986.h"
#include "hof.h"
#include "chars.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

MAKE_LEN(URI, scheme,   data->colon_s                                          )
MAKE_LEN(URI, userinfo, data->atsymbol                                         )
MAKE_LEN(URI, host,     OR(colon_p,   OR(path, OR(question, OR(pound, data->end)))))
MAKE_LEN(URI, port,     OR(              path, OR(question, OR(pound, data->end))))
MAKE_LEN(URI, path,     OR(                       question, OR(pound, data->end)))
MAKE_LEN(URI, query,    OR(                                    pound, data->end))
MAKE_LEN(URI, fragment,                                               data->end)

MAKE_GETTER(URI, scheme)
MAKE_GETTER(URI, userinfo)
MAKE_GETTER(URI, host)
MAKE_GETTER(URI, port)
MAKE_GETTER(URI, path)
MAKE_GETTER(URI, query)
MAKE_GETTER(URI, fragment)

// For the parsers other than parse_URI, the protocol is as
// follows: If the parser does not match, return NULL and don't
// modify the input string *s. If the parser matches, return a
// pointer to the first character of the match, and advance the
// input string *s to the first non-matching character.  If
// matched, the length of the match is (*s) - match.

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
                           // colon is handled in parse_userinfo
                           // parse_colon,
                           parse_sub_delims);
}
static const char *parse_userinfo(const char **s, const char **maybe_colon) {
    const char *match = parse_n_star(s, 0, parse_userinfo_char);
    // The complexity of this is necessary to identify the first colon,
    // which is used to avoid reparsing if this is a host, not a userinfo
    if ((*maybe_colon = parse_colon(s)) != NULL) {
        do {
            parse_n_star(s, 0, parse_userinfo_char);
        } while (parse_colon(s) != NULL);
    }
    return match;
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
    return parse_opt(s, 3, parse_IP_literal,
                           // IPv4address is contained by reg_name
                           // parse_IPv4address,
                           parse_reg_name);
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
        *userinfo = parse_userinfo(s, colon);
        if ((*atsymbol = parse_atsymbol(s)) != NULL) {
            // found userinfo@
            *host = parse_host(s);
        } else { // no @, it might have parsed a host
            *host = *userinfo;
            *userinfo = NULL;
            if (*colon != NULL) {
                // it parsed a host and found a colon
                // rewind since port syntax is different
                *s = *colon;
            } else if (*host == *s) {
                // it didn't parse anything because it
                // found a character like [
                *host = parse_host(s);
            }
        }
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
    URI result = { 0 };

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
        static const URI result_null = { 0 };
        result = result_null;
    }
    return result;
}
