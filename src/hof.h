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

#ifndef URI_PATH_FINDER_HOF_H
#define URI_PATH_FINDER_HOF_H

#include <stdarg.h>
#include <stddef.h>

#define MAKE_LEN(ty, field, end) \
    size_t len_##field(const ty *data) { \
        return data->field ? (end - data->field) : 0; \
    }
#define OR(a, b) \
    (data->a ? data->a : b)

#define MAKE_GETTER(ty, field) \
    char *get_##field(const ty *data, char *buf, size_t *len){ \
        size_t f_len = len_##field(data); \
        if (data->field == NULL || f_len >= *len) { \
            *len = f_len; \
            return NULL; \
        } \
        strncpy(buf, data->field, f_len); \
        buf[f_len] = '\0'; \
        return buf; \
    }

typedef const char *(*parser)(const char **);

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

#endif // URI_PATH_FINDER_HOF_H
