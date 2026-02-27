/* URIPathFinder: A simple parser for URIs
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Nate Bragg
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hof.h"
#include "chars.h"
#include "rfc_3966.h"
#include "rbtree.h"
#define RBTREE_SIZE 1000

#include <stddef.h>

#define MAKE_TEL_LEN_FROM_PARS_LEN(field) \
    size_t len_par_##field(const Tel *t) { \
        return len_##field(&t->pars); \
    }

MAKE_LEN(Tel, global_number, data->number_stop)
MAKE_LEN(Tel, local_number,  data->number_stop)
MAKE_LEN(Pars, ext, data->ext_stop)
MAKE_LEN(Pars, isdn, data->isdn_stop)
MAKE_LEN(Pars, context, data->context_stop)
MAKE_LEN(Pars, pars_1, data->pars_1_stop)
MAKE_LEN(Pars, pars_2, data->pars_2_stop)
MAKE_LEN(Pars, pars_3, data->pars_3_stop)
MAKE_LEN(Pars, pars_4, data->pars_4_stop)

MAKE_TEL_LEN_FROM_PARS_LEN(ext)
MAKE_TEL_LEN_FROM_PARS_LEN(isdn)
MAKE_TEL_LEN_FROM_PARS_LEN(context)
MAKE_TEL_LEN_FROM_PARS_LEN(pars_1)
MAKE_TEL_LEN_FROM_PARS_LEN(pars_2)
MAKE_TEL_LEN_FROM_PARS_LEN(pars_3)
MAKE_TEL_LEN_FROM_PARS_LEN(pars_4)

size_t len_pars(const Tel *t) {
    return len_pars_1(&t->pars) + len_pars_2(&t->pars) +
           len_pars_3(&t->pars) + len_pars_4(&t->pars);
}

#define MAKE_TEL_GET_FROM_PARS_GET(field) \
    char *get_par_##field(const Tel *t, char *buf, size_t *len) { \
        return get_##field(&t->pars, buf, len); \
    }

MAKE_GETTER(Tel, global_number)
MAKE_GETTER(Tel, local_number)
MAKE_GETTER(Pars, ext)
MAKE_GETTER(Pars, isdn)
MAKE_GETTER(Pars, context)
MAKE_GETTER(Pars, pars_1)
MAKE_GETTER(Pars, pars_2)
MAKE_GETTER(Pars, pars_3)
MAKE_GETTER(Pars, pars_4)

MAKE_TEL_GET_FROM_PARS_GET(ext)
MAKE_TEL_GET_FROM_PARS_GET(isdn)
MAKE_TEL_GET_FROM_PARS_GET(context)
MAKE_TEL_GET_FROM_PARS_GET(pars_1)
MAKE_TEL_GET_FROM_PARS_GET(pars_2)
MAKE_TEL_GET_FROM_PARS_GET(pars_3)
MAKE_TEL_GET_FROM_PARS_GET(pars_4)

char *get_pars(const Tel *t, char *buf, size_t *len) {
    size_t f_len = len_pars(t);
    if (f_len >= *len) {
        *len = f_len;
        return NULL;
    }
    size_t original_length = *len;
    if (get_pars_1(&t->pars, buf, len) == NULL) {
        return NULL;
    }
    size_t tmplen = original_length - *len;
    if (get_pars_2(&t->pars, &buf[*len], &tmplen) != NULL) {
        *len = *len + tmplen;
        tmplen = original_length - *len;
        if (get_pars_3(&t->pars, &buf[*len], &tmplen) != NULL) {
            *len = *len + tmplen;
            tmplen = original_length - *len;
            if (get_pars_4(&t->pars, &buf[*len], &tmplen) != NULL) {
                *len = *len + tmplen;
            }
        }
    }
    return buf;
}

/* alphanum = ALPHA / DIGIT */
static const char *parse_alphanum(const char **s) {
    return parse_opt(s, 2, parse_alpha, parse_digit);
}

/* reserved = ";" / "/" / "?" / ":" / "@" / "&" /
 *            "=" / "+" / "$" / "," */
static const char *parse_reserved(const char **s) {
    return parse_opt(s, 9,  parse_fwd_slash, parse_question,
                            parse_colon, parse_atsymbol, parse_ampersand,
                            parse_equal, parse_plus, parse_dollar, parse_comma);
}

/* mark = "-" / "_" / "." / "!" / "~" / "*" /
 *        "'" / "(" / ")" */
static const char *parse_mark(const char **s) {
    return parse_opt(s, 9, parse_dash, parse_underscore, parse_dot,
                           parse_exclamation, parse_tilde, parse_star,
                           parse_singlequote, parse_lparens, parse_rparens);
}

/* unreserved = alphanum / mark */
static const char *parse_unreserved(const char **s) {
    return parse_opt(s, 2, parse_alphanum, parse_mark);
}

/* pct-encoded = "%" HEXDIG HEXDIG */
static const char *parse_pct_encoded(const char **s) {
    return parse_cat(s, 3, parse_percent, parse_hexdig, parse_hexdig);
}

/* uric = reserved / unreserved / pct-encoded */
static const char *parse_uric(const char **s) {
    return parse_opt(s, 3, parse_reserved, parse_unreserved, parse_pct_encoded);
}

/* visual-separator = "-" / "." / "(" / ")" */
static const char *parse_visual_separator(const char **s) {
    return parse_opt(s, 4, parse_dash, parse_dot, parse_lparens, parse_rparens);
}

/* phonedigit-hex = HEXDIG / "*" / "#" / [ visual-separator ] */
static const char *parse_phonedigit_hex(const char **s) {
    return parse_opt(s, 4, parse_hexdig, parse_star, parse_pound,
                           /* brackets make no sense here since
                              it's already optional with the
                              brackets, rules invoking this one
                              can simply loop forever */
                           parse_visual_separator);
}

/* phonedigit = DIGIT / [ visual-separator ] */
static const char *parse_phonedigit(const char **s) {
    return parse_opt(s, 2, parse_digit,
                           /* brackets make no sense here since
                              it's already optional with the
                              brackets, rules invoking this one
                              can simply loop forever */
                           parse_visual_separator);
}

/* param-unreserved = "[" / "]" / "/" / ":" / "&" / "+" / "$" */
static const char *parse_param_unreserved(const char **s) {
    return parse_opt(s, 7, parse_lbracket, parse_rbracket, parse_fwd_slash,
                           parse_colon, parse_ampersand, parse_plus,
                           parse_dollar);
}

/* paramchar = param-unreserved / unreserved / pct-encoded */
static const char *parse_paramchar(const char **s) {
    return parse_opt(s, 3, parse_param_unreserved, parse_unreserved, parse_pct_encoded);
}

/* pvalue = 1*paramchar */
static const char *parse_pvalue(const char **s) {
    return parse_n_star(s, 1, parse_paramchar);
}

/* pname = 1*( alphanum / "-" ) */
static const char *parse_pname_char(const char **s) {
    return parse_opt(s, 2, parse_alphanum, parse_dash);
}
static const char *parse_pname(const char **s) {
    return parse_n_star(s, 1, parse_pname_char);
}

/* parameter = ";" pname ["=" pvalue ] */
static const char *parse_parameter(const char **s, const char **pnend) {
    /* Handle ; below */
    const char *match = parse_pname(s);
    *pnend = NULL;
    if (match != NULL) {
        *pnend = *s;
        parse_cat(s, 2, parse_equal, parse_pvalue);
    }
    return match;
}

/* toplabel = ALPHA / ALPHA *( alphanum / "-" ) alphanum
 * domainlabel = alphanum / alphanum *( alphanum / "-" ) alphanum */
static const char *parse_label_char(const char **s) {
    const char *match = parse_pname(s);
    if (match != NULL) {
        /* recheck that the last character isn't a dash */
        *s = (*s) - 1;
        if (parse_alphanum(s) == NULL) {
            *s = match;
        }
    }
    return match;
}
static const char *parse_domainlabel(const char **s) {
    const char *match = parse_alphanum(s);
    if (match != NULL) {
        parse_label_char(s);
    }
    return match;
}

/* domainname = *( domainlabel "." ) toplabel [ "." ] */
static const char *parse_domainname(const char **s) {
    const char *match = parse_domainlabel(s);
    const char *cur = match;
    const char *last_potential_toplabel = NULL;
    const char *last_potential_toplabel_stop = match;
    while (cur != NULL) {
        /* Before anything else, peel off a dot */
        const char *dot = parse_dot(s);
        if (parse_digit(&cur) == NULL) {
            /* If the label doesn't start with a digit, it
               could be either a domainlabel or the toplabel */
            last_potential_toplabel = cur;
            last_potential_toplabel_stop = *s;
        }
        if (dot == NULL) {
            /* If cur could be a toplabel, then the parse
               succeeded.  If not, then the parse failed. */
            break;
        }
        cur = parse_domainlabel(s);
    }
    /* Rewind to the last_toplabel and stop there */
    *s = last_potential_toplabel_stop;
    if (last_potential_toplabel == NULL) {
        /* If a last_toplabel was never found, fail overall */
        match = NULL;
    }
    return match;
}

/* local-number-digits = *phonedigit-hex (HEXDIG / "*" / "#") *phonedigit-hex */
static const char *parse_local_number_digits(const char **s) {
    /* Due to ambiguity of the mandatory digit / * / # inside the
       phonedigit-hex visual separators have to be removed so... */
    const char *match = parse_n_star(s, 0, parse_visual_separator);
    if (/* ... on succeess, the first character must be a digit / * / #   */
        parse_n_star(s, 1, parse_phonedigit_hex) == NULL) {
        *s = match;
        match = NULL;
    }
    return match;
}

/* global-number-digits = "+" *phonedigit DIGIT *phonedigit */
static const char *parse_global_number_digits(const char **s) {
    const char *match = parse_plus(s);
    if (match != NULL &&
        /* Due to ambiguity of the mandatory digit inside the
           phonedigit visual separators have to be removed so... */
        parse_n_star(s, 0, parse_visual_separator) != NULL &&
        /* ... on succeess, the first character must be a digit */
        parse_n_star(s, 1, parse_phonedigit) == NULL) {
        *s = match;
        match = NULL;
    }
    return match;
}


/* descriptor = domainname / global-number-digits */
static const char *parse_descriptor(const char **s) {
    return parse_opt(s, 2, parse_domainname, parse_global_number_digits);
}

/* context = ";phone-context=" descriptor */
static const char *parse_context(const char **s, const char **pnend) {
    /* Handle ; below */
    const char *match = parse_str(s, "phone-context");
    *pnend = *s;
    if (match == NULL || parse_char(s, '=') == NULL ||
                         parse_descriptor(s) == NULL) {
        *pnend = NULL;
        match = NULL;
    }
    return match;
}

/* extension = ";ext=" 1*phonedigit */
static const char *parse_extension(const char **s, const char **pnend) {
    /* Handle ; below */
    const char *match = parse_str(s, "ext");
    *pnend = *s;
    if (match == NULL || parse_char(s, '=') == NULL ||
                         parse_n_star(s, 1, parse_phonedigit) == NULL) {
        *pnend = NULL;
        match = NULL;
    }
    return match;
}

/* isdn-subaddress = ";isub=" 1*uric */
static const char *parse_isdn_subaddress(const char **s, const char **pnend) {
    /* Handle ; below */
    const char *match = parse_str(s, "isub");
    *pnend = *s;
    if (match == NULL || parse_char(s, '=') == NULL ||
                         parse_n_star(s, 1, parse_uric) == NULL) {
        *pnend = NULL;
        match = NULL;
    }
    return match;
}

/* par = parameter / extension / isdn-subaddress */
static const char *parse_par(const char **s, const char **pnend, const char **ext, const char **isdn, const char **context) {
    /* Handle ; here instead of in the individual rules */
    const char *match = parse_semicolon(s);
    *ext = NULL;
    *isdn = NULL;
    *context = NULL;
    if (match != NULL &&
        /* On first glance this is inefficient, but
         * each special parameter probably fails quickly,
         * i.e., if ext= matches but the rest doesn't,
         * then isdn and context two will fail on the first
         * character.  The only inefficiency is really an
         * error (unhandled by the grammar) where the RHS
         * of these three doesn't match, e.g., ext= isn't
         * immediately followed by a phonedigit, or isub=
         * isn't immediately followed by a uric, or
         * phone-context isn't immediately followed by an
         * alphanum, in which case it would succeed in
         * parse_parameter.  Since these fail on the first
         * character after =, the reparse is minimal.
         * Admittedly this edge case is a mess. */
        ((*ext = parse_extension(s, pnend)) == NULL) &&
        ((*isdn = parse_isdn_subaddress(s,pnend)) == NULL) &&
        /* Although context is not a part of the par rule
           we handle it here for efficiency as it would
           otherwise get parsed by parse_parameter, which
           would require reparsing. */
        ((*context = parse_context(s, pnend)) == NULL) &&
        /* Generic parameter is parsed last unlike in the
           rule, since otherwise the special parameters
           can't be efficiently identified. */
        (parse_parameter(s, pnend) == NULL)) {
        *s = match;
        pnend = NULL;
        match = NULL;
    }
    return match;
}

#define max(a, b) ({ \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _b < _a ? _a : _b; })

/* Helper for parse_local_number and parse_global_number */
static const char *parse_par_star(const char **s, Pars *result) {
    /* Technically, RFC5341 constrains the possible parameters.
       We ignore that, to future proof.  Handling these requires
       extra considerations not covered by this parser. */
    const char *match = *s;
    static const Pars result_null = { 0 };
    *result = result_null;
    const char *ptmp = NULL;
    const char *pnend = NULL;
    const char *etmp = NULL;
    const char *itmp = NULL;
    const char *ctmp = NULL;
    tree stack[RBTREE_SIZE] = {0}; /* RBTREE_SIZE should be enough, right? */
    arena ar = { .size = RBTREE_SIZE, .entries = 0, .stack = stack };
    while ((ptmp = parse_par(s, &pnend, &etmp, &itmp, &ctmp)) != NULL) {
        /* Per the spec, each parameter name must not appear more than once. */
        if (!tree_insert(ptmp + 1, pnend - ptmp - 1, &ar)) {
            /* The parser found a duplicate parameter */
            *s = match;
            *result = result_null;
            match = NULL;
            break;
        }
#ifdef RFC_3966_CHECK_ORDER
        /* NOTE: Per the spec, compliant parsers must strictly check that the
           'isdn-subaddress' or 'extension' parameters appear first, if
           present, followed by the 'context' parameter, if present, followed
           by any other parameters in lexicographical order.  However,
           for flexibility, we only check these restrictions if enabled. */
        if (result->context != NULL && result->context < result->ext ||
            result->context != NULL && result->context < result->isdn ||
            result->pars_1  != NULL && result->pars_1  < result->ext ||
            result->pars_1  != NULL && result->pars_1  < result->isdn ||
            result->pars_1  != NULL && result->context < result->context ||
            result->pars_2  != NULL ||
            result->pars_3  != NULL ||
            result->pars_4  != NULL ||
            tree_max(&stack[0])->v != ptmp) {
            *s = match;
            *result = result_null;
            match = NULL;
            break;
        }
#endif /* RFC_3966_CHECK_ORDER */

        char *lreg = max(result->pars_1,
                     max(result->pars_2,
                     max(result->pars_3,
                         result->pars_4)));
        char *lpar = max(result->ext,
                     max(result->isdn,
                     max(result->context,
                         lreg)));
        
        char **start = etmp ? &result->ext :
                       itmp ? &result->isdn :
                       ctmp ? &result->context : NULL;
                      
        char **stop  = etmp ? &result->ext_stop :
                       itmp ? &result->isdn_stop :
                       ctmp ? &result->context_stop : NULL;
                      
        if (start != NULL && *start == NULL) {
            /* This is the first occurance of a special parameter */
            *start = (char*)ptmp;
            *stop = (char*)*s;
        } else if (start != NULL) {
            /* This is a special parameter but has been seen before
               Thus this parameter list is invalid */
            *s = match;
            *result = result_null;
            match = NULL;
            break;
        } else if (lreg != NULL && lreg == lpar) {
            /* This is a regular parameter and so was the previous one
               so it doesn't edit the start, but just bumps the stop */
            stop = lreg == result->pars_1 ? &result->pars_1_stop :
                   lreg == result->pars_2 ? &result->pars_2_stop :
                   lreg == result->pars_3 ? &result->pars_3_stop :
                                            &result->pars_4_stop;
            *stop = (char*)*s;
        } else {
            /* This is a regular parameter but the previous was special
               so it must be put into the next unused pars member */
            start = lreg == NULL           ? &result->pars_1 :
                    lreg == result->pars_1 ? &result->pars_2 :
                    lreg == result->pars_2 ? &result->pars_3 :
                    lreg == result->pars_3 ? &result->pars_4 : NULL;
            stop  = lreg == NULL           ? &result->pars_1_stop :
                    lreg == result->pars_1 ? &result->pars_2_stop :
                    lreg == result->pars_2 ? &result->pars_3_stop :
                    lreg == result->pars_3 ? &result->pars_4_stop : NULL;
            *start = (char*)ptmp;
            *stop = (char*)*s;
        }
    }
    return match;
}

/* local-number = local-number-digits *par context *par */
static const char *parse_local_number(const char **s, Tel *t) {
    const char *match = parse_local_number_digits(s);
    /* Check for valid par list and context, which must be present */
    if (match != NULL) {
        t->local_number = (char*)match;
        t->number_stop = (char*)*s;
        /* Check for valid par list and context, which must be present */
        if (parse_par_star(s, &t->pars) == NULL || t->pars.context == NULL) {
            *s = match;
            match = NULL;
        }
    }
    return match;
}

/* global-number = global-number-digits *par */
static const char *parse_global_number(const char **s, Tel *t) {
    const char *match = parse_global_number_digits(s);
    if (match != NULL) {
        t->global_number = (char*)match;
        t->number_stop = (char*)*s;
        /* Check for valid par list but not context, which shouldn't be present */
        if (parse_par_star(s, &t->pars) == NULL || t->pars.context != NULL) {
            *s = match;
            match = NULL;
        }
    }
    return match;
}

/* telephone-subscriber global-number / local-number */
static const char *parse_telephone_subscriber(const char **s, Tel *t) {
    const char *match = parse_global_number(s, t);
    if (match == NULL) {
        match = parse_local_number(s, t);
    }
    return match;
}

/* telephone-uri = "tel:" telephone-subscriber */
Tel parse_telephone(const char *uri) {
    const char **s = &uri;
    Tel result = { 0 };
    if (parse_str(s, "tel:") != NULL) {
        if (parse_telephone_subscriber(s, &result) == NULL || **s != '\0') {
            static const Tel result_null = { 0 };
            result = result_null;
        }
    }
    return result;
}
