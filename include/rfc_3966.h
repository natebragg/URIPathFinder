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

#ifndef URI_PATH_FINDER_RFC_3966_H
#define URI_PATH_FINDER_RFC_3966_H

#include <stddef.h>

typedef struct Pars {
    char *ext;
    char *ext_stop;
    char *isdn;
    char *isdn_stop;
    char *context;
    char *context_stop;
    char *pars_1;
    char *pars_1_stop;
    char *pars_2;
    char *pars_2_stop;
    char *pars_3;
    char *pars_3_stop;
    char *pars_4;
    char *pars_4_stop;
} Pars;

typedef struct Tel {
    char *global_number;
    char *local_number;
    char *number_stop;
    Pars pars;
} Tel;

/* For details about parse, get, and len API, see rfc_3986.h */
Tel parse_telephone(const char *s);

char *get_global_number(const Tel *, char *, size_t *);
char *get_local_number(const Tel *, char *, size_t *);
char *get_pars(const Tel *, char *, size_t *); /* combo of pars_1/2/3/4 */
char *get_par_ext(const Tel *, char *, size_t *);
char *get_par_isdn(const Tel *, char *, size_t *);
char *get_par_context(const Tel *, char *, size_t *);
char *get_par_pars_1(const Tel *, char *, size_t *);
char *get_par_pars_2(const Tel *, char *, size_t *);
char *get_par_pars_3(const Tel *, char *, size_t *);
char *get_par_pars_4(const Tel *, char *, size_t *);

size_t len_global_number(const Tel *);
size_t len_local_number(const Tel *);
size_t len_pars(const Tel *); /* combo of pars_1/2/3/4 */
size_t len_par_ext(const Tel *);
size_t len_par_isdn(const Tel *);
size_t len_par_context(const Tel *);
size_t len_par_pars_1(const Tel *);
size_t len_par_pars_2(const Tel *);
size_t len_par_pars_3(const Tel *);
size_t len_par_pars_4(const Tel *);

#endif /* URI_PATH_FINDER_RFC_3966_H */
