#ifndef URI_PATH_FINDER_RFC_3986_H
#define URI_PATH_FINDER_RFC_3986_H

#include <stddef.h>

// A parser for the RFC 3986 URI Generic Syntax.
// The grammar is taken from Appendix A of the RFC

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

// Top-level parser for URIs.
// Takes a valid URI as a NULL terminated string.
// Returns a URI struct with each member pointing to the
// start of the respective field.
// If the URI input string is invalid, all fields of the URI
// will be NULL.  Otherwise, all parts present in the URI will
// be filled in.  Any that are missing will be NULL.
//
// NOTE: To avoid allocating, the URI object points directly into
//       the original string.  The lifetime of the URI value is
//       thus linked to the lifetime of the original string.
URI parse_URI(const char *);

// Accordingly, it's preferable to retrieve the fields of the
// URI via these getters that create a NULL-terminated copy in
// a user-supplied buffer.  This takes O(n) time, though.
//
// If the buffer is large enough to hold the field and the
// field is populated, it is transcribed, null terminated, and
// a pointer to the first character is also returned.  If the
// field is NULL, the function returns NULL, the buffer is not
// changed, and the length field is set to 0.  If the buffer
// is too small, the function returns NULL, the buffer is not
// changed, and the length field is set to the length of the
// field excluding the NULL terminating byte.
char *get_scheme(const URI *, char *, size_t *);
char *get_userinfo(const URI *, char *, size_t *);
char *get_host(const URI *, char *, size_t *);
char *get_port(const URI *, char *, size_t *);
char *get_path(const URI *, char *, size_t *);
char *get_query(const URI *, char *, size_t *);
char *get_fragment(const URI *, char *, size_t *);

// If you don't want to pay for making a copy, these
// length functions are constant time.
size_t len_scheme(const URI *);
size_t len_userinfo(const URI *);
size_t len_host(const URI *);
size_t len_port(const URI *);
size_t len_path(const URI *);
size_t len_query(const URI *);
size_t len_fragment(const URI *);

// By default RFC-3986 only handles ASCII.  If more characters
// are needed, set the functions to handle them.  They must
// return a pointer to the character if found and advance the
// argument to beyond the character. If not found, return NULL
// and do not advance the argument.
typedef const char *(*parser)(const char **);
void set_alpha_parser(parser);
void set_digit_parser(parser);

#endif
