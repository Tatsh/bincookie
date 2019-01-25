#include <stdio.h>

#include "../bincookie.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [Cookies.binarycookies files]\n", argv[0]);
        return 1;
    }

    binarycookies_t *bc;
    unsigned int i, j;

    // Output in Netscape cookies.txt format
    for (int k = 1; k < argc; k++) {
        bc = binarycookies_init(argv[k]);

        if (!bc) {
            continue;
        }

        for (i = 0; i < bc->num_pages; i++) {
            for (j = 0; j < bc->pages[i]->number_of_cookies; j++) {
                // domain, flag, path, secure, expiration, name, value
                printf("%s\t%s\t%s\t%s\t%ld\t%s\t%s\n",
                    bc->pages[i]->cookies[j]->domain ? bc->pages[i]->cookies[j]->domain : "",
                    binarycookies_domain_access_full(bc->pages[i]->cookies[j]) ? "TRUE" : "FALSE",
                    bc->pages[i]->cookies[j]->path ? bc->pages[i]->cookies[j]->path : "",
                    binarycookies_is_secure(bc->pages[i]->cookies[j]) ? "TRUE" : "FALSE",
                    bc->pages[i]->cookies[j]->expiration_time,
                    bc->pages[i]->cookies[j]->name,
                    bc->pages[i]->cookies[j]->value);
            }
        }

        binarycookies_free(bc);
    }

    return 0;
}
