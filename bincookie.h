/**
 * @file bincookie.h
 * @brief A parser for Apple's proprietary binary cookie storage format.
 *
 * Based on the Python script by Satishb3 <satishb3@securitylearn.net>
 */

#ifndef _BINCOOKIE_H
#define _BINCOOKIE_H

#ifdef __cplusplus
extern "C" {
#endif

// https://github.com/nodejs/http-parser/blob/master/http_parser.h#L32
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__) &&                               \
    (!defined(_MSC_VER) || _MSC_VER < 1600) && !defined(__WINE__)
#include <BaseTsd.h>
#include <stddef.h>
typedef unsigned __int32 uint32_t;
typedef unsigned __int8 bool;
#else
#include <stdbool.h>
#include <stdint.h>
#endif
#include <errno.h>
#include <string.h>
#include <time.h>

//! Check if a cookie has the secure bit enabled (only to be accessed over
//! HTTPS).
/*!
 * \param cookie_ptr Pointer to a bincookie_cookie_t data structure.
 */
#define bincookie_is_secure(cookie_ptr)                                       \
    (cookie_ptr->flags & secure) /**< @hideinitializer */
//! Check if a cookie can be used on all subdomains of its original domain.
/*!
 * \param cookie_ptr Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_domain_access_full(cookie_ptr)                              \
    (((char *)cookie_ptr + cookie_ptr->domain_offset)[0] ==                   \
     '.') /**< @hideinitializer */
//! Initialise a bincookie_iter_state_t structure.
/*!
 * \param s bincookie_iter_state_t structure.
 */
#define bincookie_iter_state_init(s)                                          \
    s.page_offset = 0;                                                        \
    s.page_index = 0; /**< @hideinitializer */
//! Get the domain of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_domain(c)                                                   \
    ((char *)c + c->domain_offset) /**< @hideinitializer */
//! Get the path of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_path(c)                                                     \
    ((char *)c + c->path_offset) /**< @hideinitializer                        \
                                  */
//! Get the name of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_name(c)                                                     \
    ((char *)c + c->name_offset) /**< @hideinitializer                        \
                                  */
//! Get the value of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_value(c)                                                    \
    ((char *)c + c->value_offset) /**< @hideinitializer */
//! Difference between Apple's epoch (2001-01-01) and UNIX.
#define APPLE_EPOCH_OFFSET 978307200
//! Get the expiration time of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_expiration_time(c)                                          \
    (c->expiry_date_epoch + APPLE_EPOCH_OFFSET) /**< @hideinitializer */
//! Get the creation time of a cookie.
/*!
 * \param c Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_creation_time(c)                                            \
    (c->create_date_epoch + APPLE_EPOCH_OFFSET) /**< @hideinitializer */

//! Security enabled for a cookie.
/*! A cookie can be either or these values or 2 or more values OR'd together.
 */
typedef enum {
    secure = 1, /**< Cookie can only be used with HTTPS @hideinitializer */
    http_only =
        1
        << 2, /**< Cookie cannot be accessed by JavaScript @hideinitializer */
} bincookie_flag;

//! Keeps track of iteration state when iterating cookie file pages.
typedef struct {
    uint32_t page_offset; /*!< Current offset into the buffer. */
    uint32_t page_index;  /*!< Current page. */
} bincookie_iter_state_t;

//! Cookie data structure.
typedef struct {
    uint32_t size; /*!< Size of the cookie data. */
    unsigned char unk1[4];
    bincookie_flag flags; /*!< Flags used with the cookie. */
    unsigned char unk2[4];

    uint32_t domain_offset; /*!< Offset of the domain name. */
    uint32_t name_offset;   /*!< Offset of the name of the cookie. */
    uint32_t path_offset;   /*!< Offset of the path. */
    uint32_t value_offset;  /*!< Offset of the value. */

    unsigned char unk[8];

    double expiry_date_epoch; /*!< Expiry date in Apple epoch format. */
    double create_date_epoch; /*!< Creation date in Apple epoch format. */
} bincookie_cookie_t;

//! Cookie page structure. A page can consist of 1 or more cookies.
typedef struct {
    unsigned char unk1[4]; /*!< Has unknown purpose. Seems to be always `0x0
                              0x0 0x1 0x0`. */
    uint32_t num_cookies;  /*!< Number of cookies contained in this page. */
    uint32_t cookie_offsets[]; /*!< Cookie offset values. */
} bincookie_page_t;

//! bincookie file structure.
typedef struct {
    unsigned char magic[4]; /*!< Magic: "cook" */
    uint32_t num_pages;     /*!< Number of pages in this file */
    uint32_t page_sizes[];  /*!< Page sizes (same length as number of pages) */
} bincookie_t;

//! Read a bincookie file.
/*!
 \param fin Opened file handle.
 \return Pointer to a bincookie_t structure or `NULL`.
 */
static inline bincookie_t *const bincookie_init_file(FILE *fin) {
    const long last_pos = ftell(fin);
    rewind(fin);
    char magic[4];
    size_t read = fread(magic, sizeof(magic), 1, fin);
    fseek(fin, last_pos, SEEK_SET);
    bool is_cook = magic[0] == 'c' && magic[1] == 'o' && magic[2] == 'o' &&
                   magic[3] == 'k';
    if (read != 1 || !is_cook) {
        errno = EIO;
        return NULL;
    }

    // Read entire file
    fseek(fin, 0, SEEK_END);
    size_t num_bytes = (size_t)ftell(fin);
    bincookie_t *cook = malloc(num_bytes);
    if (cook == NULL) {
        return NULL;
    }
    memset(cook, 0, num_bytes);
    rewind(fin);
    fread(cook, 1, num_bytes, fin);

    cook->num_pages = __builtin_bswap32(cook->num_pages);

    // Fix page size numbers
    for (uint32_t i = 0; i < cook->num_pages; i++) {
        *(cook->page_sizes + i) = __builtin_bswap32(*(cook->page_sizes + i));
    }

    fseek(fin, last_pos, SEEK_SET);

    return cook;
}
//! Read a bincookie file.
/*!
 \param file_path File path string.
 \return Pointer to a bincookie_t structure or `NULL`.
 */
static inline bincookie_t *const bincookie_init_path(const char *file_path) {
    FILE *binary_file = fopen(file_path, "rb");

    if (!binary_file) {
        return NULL;
    }

    bincookie_t *ret = bincookie_init_file(binary_file);
    fclose(binary_file);

    return ret;
}
//! Iterate pages of a bincookie file.
/*!
 \param bc Pointer to bincookie_t structure.
 \param s Pointer to bincookie_iter_state_t object.
 \return Pointer to a bincookie_page_t structure or `NULL`.
 */
static inline bincookie_page_t *const
bincookie_iter_pages(const bincookie_t *bc,
                     bincookie_iter_state_t *const state) {
    if (state->page_offset == 0) {
        state->page_offset = (uint32_t)(4 + sizeof(uint32_t) +
                                        (sizeof(uint32_t) * bc->num_pages));
    }
    if (state->page_index < bc->num_pages) {
        bincookie_page_t *page =
            (bincookie_page_t *)((char *)bc + state->page_offset);
        state->page_offset += bc->page_sizes[state->page_index];
        state->page_index++;
        return page;
    }
    return NULL;
}
//! Iterate cookies of a page.
/*!
 \param page Pointer to bincookie_page_t structure.
 \param i Pointer to an integer to keep track of index.
 \return Pointer to a bincookie_cookie_t structure or `NULL`.
 */
static inline bincookie_cookie_t *const
bincookie_iter_cookies(const bincookie_page_t *page, unsigned int *i) {
    if (*i < page->num_cookies) {
        bincookie_cookie_t *ptr =
            (bincookie_cookie_t *)((char *)page + page->cookie_offsets[*i]);
        (*i)++;
        return ptr;
    }
    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif // _BINCOOKIE_H
