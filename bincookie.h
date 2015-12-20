#ifndef _BINCOOKIE_H
#define _BINCOOKIE_H

#if defined(_MSC_VER)
#define uint32_t DWORD
#else
#include <stdint.h>
#endif

#include <time.h>

#define binarycookies_is_secure(cookie_ptr) (cookie_ptr->flags & secure)
#define binarycookies_domain_access_full(cookie_ptr) (cookie_ptr->domain[0] == '.')

typedef enum {
    secure = 1,
    http_only = 1 << 2,
} binarycookies_flag;

typedef struct {
    uint32_t size;
    unsigned char unk1[4];
    binarycookies_flag flags;
    unsigned char unk2[4];

    time_t creation_time;
    time_t expiration_time;
    char *domain;
    char *name;
    char *path;
    char *value;
} binarycookies_cookie_t;

typedef struct {
    unsigned char unk1[4]; // always 0x0 0x0 0x1 0x0
    uint32_t number_of_cookies;
    binarycookies_cookie_t **cookies;
} binarycookies_page_t;

typedef struct {
    unsigned char magic[4]; // "cook"
    uint32_t num_pages;
    uint32_t *page_sizes;
    char **raw_pages;
    binarycookies_page_t **pages;
} binarycookies_t;

binarycookies_t *binarycookies_init(const char *file_path);
void binarycookies_free(binarycookies_t *cfile);

#endif // _BINCOOKIE_H
