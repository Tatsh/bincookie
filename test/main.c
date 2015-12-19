#include <stdio.h>

#include "bincookie.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [Full path to Cookies.binarycookies file]\n", argv[0]);
        printf("Example: %s Cookies.binarycookies\n", argv[0]);
        return 1;
    }

    binarycookies_t *bc = binarycookies_init(argv[1]);
    unsigned int i, j;
    uint32_t flags;

    // Output in Netscape cookies.txt format
    for (i = 0; i < bc->num_pages; i++) {
        for (j = 0; j < bc->pages[i]->number_of_cookies; j++) {
            flags = bc->pages[i]->cookies[j]->flags;

            // domain, flag, path, secure, expiration, name, value
            printf("%s\t%s\t%s\t%s\t%.f\t%s\t%s\n",
                   bc->pages[i]->cookies[j]->url,
                   bc->pages[i]->cookies[j]->url[0] == '.' ? "TRUE" : "FALSE",
                   bc->pages[i]->cookies[j]->path,
                   flags == 1 || flags == 5 ? "TRUE" : "FALSE",
                   bc->pages[i]->cookies[j]->expiration_date,
                   bc->pages[i]->cookies[j]->name,
                   bc->pages[i]->cookies[j]->value);
        }
    }

    binarycookies_free(bc);

    return 0;
}
