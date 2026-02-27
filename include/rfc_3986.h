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

#ifndef URI_PATH_FINDER_RFC_3986_H
#define URI_PATH_FINDER_RFC_3986_H

#include <stddef.h>

/* A parser for the RFC 3986 URI Generic Syntax.
 * The grammar is taken from Appendix A of the RFC */

typedef struct URI {
    char *scheme;
    char *colon_s;
    char *slash;
    char *userinfo;
    char *atsymbol;
    char *host;
    char *colon_p;
    char *port;
    char *path;
    char *question;
    char *query;
    char *pound;
    char *fragment;
    char *end;
} URI;

/* Top-level parser for URIs.
 * Takes a valid URI as a NULL terminated string.
 * Returns a URI struct with each member pointing to the
 * start of the respective field.
 * If the URI input string is invalid, all fields of the URI
 * will be NULL.  Otherwise, all parts present in the URI will
 * be filled in.  Any that are missing will be NULL.
 *
 * NOTE: To avoid allocating, the URI object points directly into
 *       the original string.  The lifetime of the URI value is
 *       thus linked to the lifetime of the original string. */
URI parse_URI(const char *);

/* Accordingly, it's preferable to retrieve the fields of the
 * URI via these getters that create a NULL-terminated copy in
 * a user-supplied buffer.  This takes O(n) time, though.
 *
 * If the buffer is large enough to hold the field and the
 * field is populated, it is transcribed, null terminated, and
 * a pointer to the first character is also returned.  If the
 * field is NULL, the function returns NULL, the buffer is not
 * changed, and the length field is set to 0.  If the buffer
 * is too small, the function returns NULL, the buffer is not
 * changed, and the length field is set to the length of the
 * field excluding the NULL terminating byte. */
char *get_scheme(const URI *, char *, size_t *);
char *get_userinfo(const URI *, char *, size_t *);
char *get_host(const URI *, char *, size_t *);
char *get_port(const URI *, char *, size_t *);
char *get_path(const URI *, char *, size_t *);
char *get_query(const URI *, char *, size_t *);
char *get_fragment(const URI *, char *, size_t *);

/* If you don't want to pay for making a copy, these
 * length functions are constant time. */
size_t len_scheme(const URI *);
size_t len_userinfo(const URI *);
size_t len_host(const URI *);
size_t len_port(const URI *);
size_t len_path(const URI *);
size_t len_query(const URI *);
size_t len_fragment(const URI *);

#endif /* URI_PATH_FINDER_RFC_3986_H */
