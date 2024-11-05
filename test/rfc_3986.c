#include "rfc_3986.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

static int failures = 0;

#define NULL_CHECK(id) (result.id && !p_##id) || (!result.id && p_##id)
#define BAD_LEN_CHECK(id, len) result.id && len != (int)strlen(p_##id)
#define BAD_COMPARE(id) result.id && strncmp(result.id, p_##id, strlen(p_##id))
void test_uri(char *p_url, char *p_scheme, char *p_userinfo, char *p_host, char *p_port, char *p_path, char *p_query, char *p_fragment)
{
    URI result = parse_URI(p_url);
    int scheme_len   = len_scheme(&result);
    int userinfo_len = len_userinfo(&result);
    int host_len     = len_host(&result);
    int port_len     = len_port(&result);
    int path_len     = len_path(&result);
    int query_len    = len_query(&result);
    int fragment_len = len_fragment(&result);
    if (NULL_CHECK(scheme)   || BAD_LEN_CHECK(scheme,   scheme_len)   || BAD_COMPARE(scheme) ||
        NULL_CHECK(userinfo) || BAD_LEN_CHECK(userinfo, userinfo_len) || BAD_COMPARE(userinfo) ||
        NULL_CHECK(host)     || BAD_LEN_CHECK(host,     host_len)     || BAD_COMPARE(host)  ||
        NULL_CHECK(port)     || BAD_LEN_CHECK(port,     port_len)     || BAD_COMPARE(port)  ||
        NULL_CHECK(path)     || BAD_LEN_CHECK(path,     path_len)     || BAD_COMPARE(path)  ||
        NULL_CHECK(query)    || BAD_LEN_CHECK(query,    query_len)    || BAD_COMPARE(query) ||
        NULL_CHECK(fragment) || BAD_LEN_CHECK(fragment, fragment_len) || BAD_COMPARE(fragment)) {
        printf("Failed for URI: %s\n", p_url);
        printf("Expected - scheme: %s, userinfo: %s, host: %s, port: %s, path: %s, query: %s, fragment: %s\n",
               p_scheme ? p_scheme : "NULL",
               p_userinfo ? p_userinfo : "NULL",
               p_host ? p_host : "NULL",
               p_port ? p_port : "NULL",
               p_path ? p_path : "NULL",
               p_query ? p_query : "NULL",
               p_fragment ? p_fragment : "NULL");
        printf("Output   - scheme: %.*s, userinfo: %.*s, host: %.*s, port: %.*s, path: %.*s, query: %.*s, fragment: %.*s\n",
               result.scheme   ? scheme_len   : 4, result.scheme   ? result.scheme   : "NULL",
               result.userinfo ? userinfo_len : 4, result.userinfo ? result.userinfo : "NULL",
               result.host     ? host_len     : 4, result.host     ? result.host     : "NULL",
               result.port     ? port_len     : 4, result.port     ? result.port     : "NULL",
               result.path     ? path_len     : 4, result.path     ? result.path     : "NULL",
               result.query    ? query_len    : 4, result.query    ? result.query    : "NULL",
               result.fragment ? fragment_len : 4, result.fragment ? result.fragment : "NULL");
        failures++;
    }
}

int main() {
    // Basic URIs
    test_uri("http://example.com", "http", NULL, "example.com", NULL, "", NULL, NULL);
    test_uri("http://example.com/", "http", NULL, "example.com", NULL, "/", NULL, NULL);
    test_uri("http://example.com:80/path", "http", NULL, "example.com", "80", "/path", NULL, NULL);
    test_uri("http://example.com:80/path?query", "http", NULL, "example.com", "80", "/path", "query", NULL);
    test_uri("http://example.com:80/path?query#fragment", "http", NULL, "example.com", "80", "/path", "query", "fragment");
    test_uri("http://example.com/path", "http", NULL, "example.com", NULL, "/path", NULL, NULL);
    test_uri("http://example.com/path/", "http", NULL, "example.com", NULL, "/path/", NULL, NULL);
    test_uri("http://example.com/path/to/resource", "http", NULL, "example.com", NULL, "/path/to/resource", NULL, NULL);
    test_uri("ftp://ftp.example.com/resource", "ftp", NULL, "ftp.example.com", NULL, "/resource", NULL, NULL);
    test_uri("git://example.com/user/repo.git", "git", NULL, "example.com", NULL, "/user/repo.git", NULL, NULL);

    // URIs with both query and fragment
    test_uri("http://example.com/?query#fragment", "http", NULL, "example.com", NULL, "/", "query", "fragment");
    test_uri("http://example.com/path/to/resource?param=value#fragment", "http", NULL, "example.com", NULL, "/path/to/resource", "param=value", "fragment");
    test_uri("http://example.com/path/to/resource?query#fragment", "http", NULL, "example.com", NULL, "/path/to/resource", "query", "fragment");
    test_uri("http://example.com/path?query#fragment", "http", NULL, "example.com", NULL, "/path", "query", "fragment");
    test_uri("http://example.com?query#fragment", "http", NULL, "example.com", NULL, "", "query", "fragment");

    // Schemes other than HTTP
    // No specific parsing of RFC-6068
    test_uri("mailto:user@example.com", "mailto", NULL, NULL, NULL, "user@example.com", NULL, NULL);
    // No specific parsing of RFC-3966
    test_uri("tel:+1234567890", "tel", NULL, NULL, NULL, "+1234567890", NULL, NULL);
    // No specific parsing of RFC-8141
    test_uri("urn:ietf:rfc:3986", "urn", NULL, NULL, NULL, "ietf:rfc:3986", NULL, NULL);
    // These three appear at first glance to be invalid, but they are actually well formed URIs
    test_uri("http:/example.com", "http", NULL, NULL, NULL, "/example.com", NULL, NULL);
    test_uri("http:example.com", "http", NULL, NULL, NULL, "example.com", NULL, NULL);
    test_uri("http:///path", "http", NULL, "", NULL, "/path", NULL, NULL);
    // Useless, but valid
    test_uri("http://", "http", NULL, "", NULL, "", NULL, NULL);
    test_uri("http://@", "http", "", "", NULL, "", NULL, NULL);

    // URIs with userinfo
    test_uri("ftp://user@ftp.example.com", "ftp", "user", "ftp.example.com", NULL, "", NULL, NULL);
    test_uri("ftp://user:pass@ftp.example.com", "ftp", "user:pass", "ftp.example.com", NULL, "", NULL, NULL);
    test_uri("ftp://user:pass@ftp.example.com:21", "ftp", "user:pass", "ftp.example.com", "21", "", NULL, NULL);
    test_uri("http://user@example.com", "http", "user", "example.com", NULL, "", NULL, NULL);
    test_uri("http://user:pass@example.com", "http", "user:pass", "example.com", NULL, "", NULL, NULL);
    test_uri("http://user:pass@example.com:80", "http", "user:pass", "example.com", "80", "", NULL, NULL);

    // Uncommon ports and complex paths
    test_uri("ftp://example.com:2121/files", "ftp", NULL, "example.com", "2121", "/files", NULL, NULL);
    test_uri("http://127.0.0.1:8080/resource", "http", NULL, "127.0.0.1", "8080", "/resource", NULL, NULL);
    test_uri("http://example.com:1234/path/to/page", "http", NULL, "example.com", "1234", "/path/to/page", NULL, NULL);
    test_uri("http://localhost:8080/test", "http", NULL, "localhost", "8080", "/test", NULL, NULL);
    test_uri("https://secure.example.com:8443", "https", NULL, "secure.example.com", "8443", "", NULL, NULL);

    // URIs with complex paths and query strings
    test_uri("http://example.com/path/to/page/?param=value&another=thing", "http", NULL, "example.com", NULL, "/path/to/page/", "param=value&another=thing", NULL);
    test_uri("http://example.com/path/to/page?param1=value1&param2=value2", "http", NULL, "example.com", NULL, "/path/to/page", "param1=value1&param2=value2", NULL);
    test_uri("http://example.com/path;params?query=1", "http", NULL, "example.com", NULL, "/path;params", "query=1", NULL);
    test_uri("http://example.com/path?encoded%20query=space%20test", "http", NULL, "example.com", NULL, "/path", "encoded%20query=space%20test", NULL);
    test_uri("http://example.com/path?query_with_symbols=@!$&'()*+,%3D", "http", NULL, "example.com", NULL, "/path", "query_with_symbols=@!$&'()*+,%3D", NULL);

    // URIs with empty path or authority
    // No specific parsing of RFC-8089
    test_uri("file:///", "file", NULL, "", NULL, "/", NULL, NULL);
    test_uri("http://:@host", "http", ":", "host", NULL, "", NULL, NULL);
    test_uri("http://example.com#", "http", NULL, "example.com", NULL, "", NULL, "");
    test_uri("http://example.com?", "http", NULL, "example.com", NULL, "", "", NULL);
    test_uri("http://user@host", "http", "user", "host", NULL, "", NULL, NULL);

    // URIs with extra slashes in path
    test_uri("http://example.com////slashes#fragment", "http", NULL, "example.com", NULL, "////slashes", NULL, "fragment");
    test_uri("http://example.com///triple///slashes", "http", NULL, "example.com", NULL, "///triple///slashes", NULL, NULL);
    test_uri("http://example.com//double//slashes/", "http", NULL, "example.com", NULL, "//double//slashes/", NULL, NULL);
    test_uri("http://example.com/path//to//resource", "http", NULL, "example.com", NULL, "/path//to//resource", NULL, NULL);
    test_uri("http://example.com:8080//another//test", "http", NULL, "example.com", "8080", "//another//test", NULL, NULL);

    // URIs with file scheme
    // No specific parsing of RFC-8089
    test_uri("file:///C:/path/to/windows/file", "file", NULL, "", NULL, "/C:/path/to/windows/file", NULL, NULL);
    test_uri("file:C:/path/to/windows/file", "file", NULL, NULL, NULL, "C:/path/to/windows/file", NULL, NULL);
    test_uri("file:///home/user/file.txt", "file", NULL, "", NULL, "/home/user/file.txt", NULL, NULL);
    test_uri("file://localhost/C:/path/to/file", "file", NULL, "localhost", NULL, "/C:/path/to/file", NULL, NULL);
    test_uri("file://localhost/path/to/file", "file", NULL, "localhost", NULL, "/path/to/file", NULL, NULL);

    // URIs with fragment containing special characters
    test_uri("http://example.com/#data%3Afragment", "http", NULL, "example.com", NULL, "/", NULL, "data%3Afragment");
    test_uri("http://example.com/#special@chars!", "http", NULL, "example.com", NULL, "/", NULL, "special@chars!");
    test_uri("http://example.com/path#unicode%F0%9F%98%80", "http", NULL, "example.com", NULL, "/path", NULL, "unicode%F0%9F%98%80");
    test_uri("http://example.com/path?query#%23encoded", "http", NULL, "example.com", NULL, "/path", "query", "%23encoded");
    test_uri("http://example.com/path?query#frag%20ment", "http", NULL, "example.com", NULL, "/path", "query", "frag%20ment");

    // URIs with fragments
    test_uri("http://example.com#fragment", "http", NULL, "example.com", NULL, "", NULL, "fragment");
    test_uri("http://example.com/#fragment", "http", NULL, "example.com", NULL, "/", NULL, "fragment");
    test_uri("http://example.com/path#fragment", "http", NULL, "example.com", NULL, "/path", NULL, "fragment");
    test_uri("http://example.com/path/to/resource#fragment", "http", NULL, "example.com", NULL, "/path/to/resource", NULL, "fragment");
    test_uri("http://example.com/path/to/resource#section1", "http", NULL, "example.com", NULL, "/path/to/resource", NULL, "section1");

    // URIs with multiple consecutive colons in authority or path
    test_uri("http://example.com/path::path2", "http", NULL, "example.com", NULL, "/path::path2", NULL, NULL);
    test_uri("http://example.com/path:subpath:subsubpath", "http", NULL, "example.com", NULL, "/path:subpath:subsubpath", NULL, NULL);
    test_uri("http://user::pass@example.com:80", "http", "user::pass", "example.com", "80", "", NULL, NULL);

    // URIs with multiple slashes
    test_uri("ftp://ftp.example.com///file", "ftp", NULL, "ftp.example.com", NULL, "///file", NULL, NULL);
    test_uri("http://example.com//////path", "http", NULL, "example.com", NULL, "//////path", NULL, NULL);
    test_uri("http://example.com//double//slash", "http", NULL, "example.com", NULL, "//double//slash", NULL, NULL);
    test_uri("http://example.com/path///to///resource", "http", NULL, "example.com", NULL, "/path///to///resource", NULL, NULL);
    test_uri("http://example.com:8080///multiple/slashes", "http", NULL, "example.com", "8080", "///multiple/slashes", NULL, NULL);

    // URIs with paths that include encoded delimiters
    test_uri("http://example.com/%2F%3Fpath%3Dquery", "http", NULL, "example.com", NULL, "/%2F%3Fpath%3Dquery", NULL, NULL);
    test_uri("http://example.com/%2Fpath#frag", "http", NULL, "example.com", NULL, "/%2Fpath", NULL, "frag");
    test_uri("http://example.com/%2Fpath%2F", "http", NULL, "example.com", NULL, "/%2Fpath%2F", NULL, NULL);
    test_uri("http://example.com/%2Fpath?param=%2Fvalue", "http", NULL, "example.com", NULL, "/%2Fpath", "param=%2Fvalue", NULL);
    test_uri("http://example.com/path%2Fto%2Fresource", "http", NULL, "example.com", NULL, "/path%2Fto%2Fresource", NULL, NULL);

    // URIs with port numbers
    test_uri("ftp://ftp.example.com:21", "ftp", NULL, "ftp.example.com", "21", "", NULL, NULL);
    test_uri("http://example.com:1234/path", "http", NULL, "example.com", "1234", "/path", NULL, NULL);
    test_uri("http://example.com:80", "http", NULL, "example.com", "80", "", NULL, NULL);
    test_uri("http://example.com:8080", "http", NULL, "example.com", "8080", "", NULL, NULL);
    test_uri("https://example.com:443", "https", NULL, "example.com", "443", "", NULL, NULL);

    // URIs with query strings
    test_uri("http://example.com/?query", "http", NULL, "example.com", NULL, "/", "query", NULL);
    test_uri("http://example.com/path/to/resource?param=value", "http", NULL, "example.com", NULL, "/path/to/resource", "param=value", NULL);
    test_uri("http://example.com/path/to/resource?query", "http", NULL, "example.com", NULL, "/path/to/resource", "query", NULL);
    test_uri("http://example.com/path?query", "http", NULL, "example.com", NULL, "/path", "query", NULL);
    test_uri("http://example.com?query", "http", NULL, "example.com", NULL, "", "query", NULL);

    // URIs with relative paths
    test_uri("http://example.com/././a/./b", "http", NULL, "example.com", NULL, "/././a/./b", NULL, NULL);
    test_uri("http://example.com/a/b/../../c", "http", NULL, "example.com", NULL, "/a/b/../../c", NULL, NULL);
    test_uri("http://example.com/path/../up/one", "http", NULL, "example.com", NULL, "/path/../up/one", NULL, NULL);
    test_uri("http://example.com/path/./to/./resource", "http", NULL, "example.com", NULL, "/path/./to/./resource", NULL, NULL);
    test_uri("http://example.com/path/dir/..", "http", NULL, "example.com", NULL, "/path/dir/..", NULL, NULL);

    // IPv6 URIs
    test_uri("http://[2001:db8::1]", "http", NULL, "[2001:db8::1]", NULL, "", NULL, NULL);
    test_uri("http://[2001:db8::1]:8080", "http", NULL, "[2001:db8::1]", "8080", "", NULL, NULL);
    test_uri("http://[::1]", "http", NULL, "[::1]", NULL, "", NULL, NULL);
    test_uri("http://[::1]/path", "http", NULL, "[::1]", NULL, "/path", NULL, NULL);
    test_uri("http://[::1]:8080", "http", NULL, "[::1]", "8080", "", NULL, NULL);

    // URIs with unusual schemes
    test_uri("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ==", "data", NULL, NULL, NULL, "text/plain;base64,SGVsbG8sIFdvcmxkIQ==", NULL, NULL);
    test_uri("irc://irc.example.com/channel", "irc", NULL, "irc.example.com", NULL, "/channel", NULL, NULL);
    // No specific parsing of magnet links
    test_uri("magnet:?xt=urn:btih:abcdef&dn=example", "magnet", NULL, NULL, NULL, "", "xt=urn:btih:abcdef&dn=example", NULL);
    // RFC-3261 is not handled correctly; it's not clear how to do this
    // test_uri("sip:user@domain.com", "sip", "user", "domain.com", NULL, NULL, NULL, NULL);
    // There is a draft RFC for URIs for SFTP and SSH
    test_uri("ssh://user@server.example.com:22", "ssh", "user", "server.example.com", "22", "", NULL, NULL);

    // Encoded characters in paths
    test_uri("http://example.com/%E2%98%83", "http", NULL, "example.com", NULL, "/%E2%98%83", NULL, NULL);
    test_uri("http://example.com/emoji/%F0%9F%98%81", "http", NULL, "example.com", NULL, "/emoji/%F0%9F%98%81", NULL, NULL);
    test_uri("http://example.com/path%20with%20spaces", "http", NULL, "example.com", NULL, "/path%20with%20spaces", NULL, NULL);
    test_uri("http://example.com/path/with/special%40character", "http", NULL, "example.com", NULL, "/path/with/special%40character", NULL, NULL);

    // Edge case: URIs with unusual character combinations
    test_uri("http://example.com:80/pa%20th/?q=a%20b#f%23g", "http", NULL, "example.com", "80", "/pa%20th/", "q=a%20b", "f%23g");
    test_uri("http://user:pass@host.com:8080/a/b/../c/./d/?x#y", "http", "user:pass", "host.com", "8080", "/a/b/../c/./d/", "x", "y");
    test_uri("http://user@host.com:1234/p@th/?query@", "http", "user", "host.com", "1234", "/p@th/", "query@", NULL);
    test_uri("https://host.com/!$%26'()*+,-./:;=?@_~#", "https", NULL, "host.com", NULL, "/!$%26'()*+,-./:;=", "@_~", "");
    test_uri("mailto:user@%20example.com", "mailto", NULL, NULL, NULL, "user@%20example.com", NULL, NULL);

    // Edge cases with userinfo containing symbols
    test_uri("ftp://an%40n%24%40nymous@ftp.example.com", "ftp", "an%40n%24%40nymous", "ftp.example.com", NULL, "", NULL, NULL);
    test_uri("http://user%3Aname@example.com", "http", "user%3Aname", "example.com", NULL, "", NULL, NULL);
    test_uri("http://user:pa$$word@example.com", "http", "user:pa$$word", "example.com", NULL, "", NULL, NULL);
    test_uri("http://user:pass@host.com:8080#frag", "http", "user:pass", "host.com", "8080", "", NULL, "frag");
    test_uri("https://user:@host.com", "https", "user:", "host.com", NULL, "", NULL, NULL);

    // Internationalized domain names (IDN)
    // NOTE: these do not work; the RFC only specifies ascii characters
    //       to test these yourself, use set_alpha_parser with a parser that handles unicode
    // test_uri("http://example.com/中文", "http", NULL, "example.com", NULL, "/中文", NULL, NULL);
    // test_uri("http://example.भारत", "http", NULL, "example.भारत", NULL, "", NULL, NULL);
    // test_uri("http://example.中国", "http", NULL, "example.中国", NULL, "", NULL, NULL);
    // test_uri("http://www.пример.рф", "http", NULL, "www.пример.рф", NULL, "", NULL, NULL);
    // test_uri("http://xn--exmple-cua.com", "http", NULL, "xn--exmple-cua.com", NULL, "", NULL, NULL);
    // test_uri("http://例子.测试", "http", NULL, "例子.测试", NULL, "", NULL, NULL);

    // URIs with reserved characters in query
    test_uri("http://example.com/path?key1=val1&key2=val2@chars", "http", NULL, "example.com", NULL, "/path", "key1=val1&key2=val2@chars", NULL);
    test_uri("http://example.com/path?query=needs%3Dencoding", "http", NULL, "example.com", NULL, "/path", "query=needs%3Dencoding", NULL);
    test_uri("http://example.com/path?query=special%20chars!@$", "http", NULL, "example.com", NULL, "/path", "query=special%20chars!@$", NULL);
    test_uri("http://example.com/path?query=val!@$&*()-_=+", "http", NULL, "example.com", NULL, "/path", "query=val!@$&*()-_=+", NULL);

    // Invalid URIs (all should result in NULL)
    test_uri("://example.com", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_uri("http://example.com/path?query=[brackets]", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_uri("ftp://user@ftp.example.com::21/files", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_uri("http://user:pass@host.com:80:443", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_uri("https://example.com:443:/path", NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    printf("Total failures: %d\n", failures);
    return 0;
}
