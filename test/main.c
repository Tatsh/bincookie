#include <stdio.h>

#include "bincookie.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [Full path to Cookies.binarycookies file]\n", argv[0]);
        printf("Example: %s Cookies.binarycookies\n", argv[0]);
        return 1;
    }

    binarycookies_t *bc = binarycookies_init(argv[1]);
    binarycookies_free(bc);

    return 0;
}
