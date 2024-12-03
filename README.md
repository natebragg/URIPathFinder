# URIPathFinder
A simple parser for URIs.

This library implements RFC-3986 and RFC-3966.

It is straightforward to use:

```C
#include <rfc_3986.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("Usage: %s url\n", argv[0]);
        return 1;
    }
    URI result = parse_URI(argv[1]);
    int scheme_len   = len_scheme(&result);
    int userinfo_len = len_userinfo(&result);
    int host_len     = len_host(&result);
    int port_len     = len_port(&result);
    int path_len     = len_path(&result);
    int query_len    = len_query(&result);
    int fragment_len = len_fragment(&result);
    printf("scheme: %.*s, userinfo: %.*s, host: %.*s, port: %.*s, path: %.*s, query: %.*s, fragment: %.*s\n",
           result.scheme   ? scheme_len   : 4, result.scheme   ? result.scheme   : "NULL",
           result.userinfo ? userinfo_len : 4, result.userinfo ? result.userinfo : "NULL",
           result.host     ? host_len     : 4, result.host     ? result.host     : "NULL",
           result.port     ? port_len     : 4, result.port     ? result.port     : "NULL",
           result.path     ? path_len     : 4, result.path     ? result.path     : "NULL",
           result.query    ? query_len    : 4, result.query    ? result.query    : "NULL",
           result.fragment ? fragment_len : 4, result.fragment ? result.fragment : "NULL");
    return 0;
}
```

It strictly follows the grammar given in the RFC, breaking the URI into its
component parts and delimiters.  It does not allocate or copy strings by
default.  In addition to the `parse_URI` and `len_*` functions shown in the
example above, it provides `get_*` functions that copy the field into a user
supplied buffer.  By default it only allows ASCII alphanumeric characters, but
the library gives two functions `set_alpha_parser` and `set_digit_parser` to
use another character set ( utf-8, etc.)

RFC 3966 has a similar interface, invoked using `parse_telephone`.
