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

#ifndef URI_PATH_FINDER_CHARS_H
#define URI_PATH_FINDER_CHARS_H

#include <stddef.h>

// Match a single character
static const char *parse_char(const char **s, char c) {
    const char *match = NULL;
    if(**s == c) {
        match = *s;
        *s = (*s) + 1;
    }
    return match;
}

static const char *parse_str(const char **s, const char *c) {
    const char *match = NULL;
    size_t i = 0;
    for (;c[i] != '\0' && c[i] == (*s)[i]; i++);
    if (c[i] == '\0') {
        match = *s;
        *s = (*s) + i;
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

// By default RFC-3986 etc. only handle ASCII.  If more characters
// are needed, set the functions to handle them.  They must
// return a pointer to the character if found and advance the
// argument to beyond the character. If not found, return NULL
// and do not advance the argument.
static parser alpha_parser = NULL;
static parser digit_parser = NULL;

void set_alpha_parser(parser p) {
    alpha_parser = p;
}

void set_digit_parser(parser p) {
    digit_parser = p;
}

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

#endif // URI_PATH_FINDER_CHARS_H
