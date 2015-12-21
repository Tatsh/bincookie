#ifndef _BINCOOKIE_H
#define _BINCOOKIE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__) && \
  (!defined(_MSC_VER) || _MSC_VER<1600) && !defined(__WINE__)
#include <BaseTsd.h>
#include <stddef.h>
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

// http://www.transmissionzero.co.uk/computing/building-dlls-with-mingw/
#if defined(_WIN32) && defined(WINDLL)
#define ADDAPI __declspec(dllexport)
#else
#define ADDAPI
#endif

// Disable warnings about secure CRT, but still enable compatible signature versions
// http://stackoverflow.com/a/119752/374110
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <time.h>

#define binarycookies_is_secure(cookie_ptr) (cookie_ptr->flags & secure)
#define binarycookies_domain_access_full(cookie_ptr) (cookie_ptr->domain && cookie_ptr->domain[0] == '.')

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

ADDAPI binarycookies_t *binarycookies_init(const char *file_path);
ADDAPI void binarycookies_free(binarycookies_t *cfile);

#ifdef __cplusplus
}
#endif

#endif // _BINCOOKIE_H
