/**
 * @file bincookie.h
 * @brief A parser for Apple's proprietary binary cookie storage format.
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
#else
#include <stdint.h>
#endif
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

#if defined(_WIN32)
#define ADDAPI __declspec(dllexport)
#else
#define ADDAPI
#endif

//! Read a bincookie file.
/*!
 \param file_path File path string.
 \return Pointer to a bincookie_t structure or `NULL`.
 */
ADDAPI bincookie_t *const bincookie_init(const char *file_path);
//! Iterate pages of a bincookie file.
/*!
 \param bc Pointer to bincookie_t structure.
 \param s Pointer to bincookie_iter_state_t object.
 \return Pointer to a bincookie_page_t structure or `NULL`.
 */
ADDAPI bincookie_page_t *const
bincookie_iter_pages(const bincookie_t *bc, bincookie_iter_state_t *const s);
//! Iterate cookies of a page.
/*!
 \param page Pointer to bincookie_page_t structure.
 \param i Pointer to an integer to keep track of index.
 \return Pointer to a bincookie_cookie_t structure or `NULL`.
 */
ADDAPI bincookie_cookie_t *const
bincookie_iter_cookies(const bincookie_page_t *page, unsigned int *i);

#ifdef __cplusplus
}
#endif

#endif // _BINCOOKIE_H
