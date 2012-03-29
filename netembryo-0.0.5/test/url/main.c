#include "url.h"
#include <stdio.h>

void test_url(char * url, char * protocol, char * host, char * port, char * path)
{
    Url turl;

    Url_init(&turl, url);

    if (protocol != turl.protocol) {
        if ( (protocol == NULL) || (turl.protocol == NULL) || (strcmp(turl.protocol, protocol) != 0) )
            printf("Url test failed on protocol check for %s\n", url);
    }

    if (host != turl.hostname) {
        if ( (host == NULL) || (turl.hostname == NULL) || (strcmp(turl.hostname, host) != 0) )
            printf("Url test failed on host check for %s\n", url);
    }

    if (port != turl.port) {
        if ( (port == NULL) || (turl.port == NULL) || (strcmp(turl.port, port) != 0) )
            printf("Url test failed on port check for %s\n", url);
    }

    if (path != turl.path) {
        if ( (path == NULL) || (turl.path == NULL) || (strcmp(turl.path, path) != 0) )
            printf("Url test failed on path check for %s\n", url);
    }

    Url_destroy(&turl);
}

void url_test_set()
{
    test_url("rtsp://this.is.a.very.long.url:this_should_be_the_port/this/is/a/path/to/file.wmv",
             "rtsp", "this.is.a.very.long.url", "this_should_be_the_port", "this/is/a/path/to/file.wmv");

    test_url("rtsp://this.is.the.host/this/is/the/path.avi", "rtsp", "this.is.the.host", NULL, "this/is/the/path.avi");
    test_url("host:80/file.wmv", NULL, "host", "80", "file.wmv");
    test_url("host/file.wmv", NULL, "host", NULL, "file.wmv");
    test_url("host", NULL, "host", NULL, NULL);
    test_url("rtsp://host", "rtsp", "host", NULL, NULL);
    test_url("rtsp://host:port", "rtsp", "host", "port", NULL);
}

int main()
{
    printf("Running url parsing test suite\n");
    url_test_set();
    printf("DONE\n");
    return 0;
}
