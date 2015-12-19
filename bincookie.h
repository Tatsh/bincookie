#ifndef _BINCOOKIE_H
#define _BINCOOKIE_H

#include <stdint.h>

#define binarycookies_is_secure(cookie_ptr) (cookie_ptr->flags == secure || cookie_ptr->flags == secure_http_only)

typedef enum {
    secure = 1,
    http_only = 1 << 2,
    secure_http_only = 5,
} binarycookies_flag;

typedef struct {
    uint32_t size;
    unsigned char unk1[4];
    binarycookies_flag flags;
    unsigned char unk2[4];

    char flags_str[16];
    double creation_date;
    char creation_date_str[30];
    double expiration_date;
    char expiration_date_str[30];
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
    uint32_t num_pages; // big endian
    uint32_t *page_sizes; // size: num_pages * sizeof(int), each size is big endian
    char **raw_pages;
    binarycookies_page_t **pages;
} binarycookies_t;

binarycookies_t *binarycookies_init(const char *file_path);
void binarycookies_free(binarycookies_t *cfile);

#endif // _BINCOOKIE_H
