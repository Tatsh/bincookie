#include <stdio.h>
#include <stdlib.h>

#include <bincookie.h>

//! Entry point.
/*!
 * \param argc Argument count.
 * \param argv Argument vector.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [Cookies.bincookie files]\n", argv[0]);
        return 1;
    }

    bincookie_t *bc;
    bincookie_page_t *page;
    bincookie_cookie_t *cookie;
    bincookie_iter_state_t state;
    unsigned int cookie_index = 0;

    // Output in Netscape cookies.txt format
    for (int k = 1; k < argc; k++) {
        bc = bincookie_init_path(argv[k]);

        if (!bc) {
            continue;
        }

        bincookie_iter_state_init(state);

        while ((page = bincookie_iter_pages(bc, &state)) != NULL) {
            while ((cookie = bincookie_iter_cookies(page, &cookie_index)) != NULL) {
                printf("%s\t%s\t%s\t%.0f\t%s\t%s\n",
                       bincookie_domain(cookie),
                       bincookie_domain_access_full(cookie) ? "TRUE" : "FALSE",
                       bincookie_path(cookie),
                       bincookie_expiration_time(cookie),
                       bincookie_name(cookie),
                       bincookie_value(cookie));
            }
            cookie_index = 0;
        }

        free(bc);
    }

    return 0;
}
