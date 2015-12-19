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

    for (i = 0; i < bc->num_pages; i++) {
        for (j = 0; j < bc->pages[i]->number_of_cookies; j++) {
            fprintf(stderr, "%s=%s; %s; %s; %s; expires %s\n", bc->pages[i]->cookies[j]->name, bc->pages[i]->cookies[j]->value, bc->pages[i]->cookies[j]->url, bc->pages[i]->cookies[j]->path, bc->pages[i]->cookies[j]->flags_str, bc->pages[i]->cookies[j]->expiration_date_str);
        }
    }

    binarycookies_free(bc);

    return 0;
}
