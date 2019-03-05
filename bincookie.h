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

//! Check if a cookie has the secure bit enabled (only to be accessed over HTTPS).
/*!
 * \param cookie_ptr Pointer to a bincookie_cookie_t data structure.
 */
#define bincookie_is_secure(cookie_ptr) (cookie_ptr->flags & secure) /**< @hideinitializer */
//! Check if a cookie can be used on all subdomains of its original domain.
/*!
 * \param cookie_ptr Pointer to a bincookie_cookie_t structure.
 */
#define bincookie_domain_access_full(cookie_ptr) (cookie_ptr->domain && cookie_ptr->domain[0] == '.') /**< @hideinitializer */

//! Security enabled for a cookie.
/*! A cookie can be either or these values or 2 or more values OR'd together. */
typedef enum {
    secure = 1,         /**< Cookie can only be used with HTTPS @hideinitializer */
    http_only = 1 << 2, /**< Cookie cannot be accessed by JavaScript @hideinitializer */
} bincookie_flag;

//! Cookie data structure.
typedef struct {
    uint32_t size;
    unsigned char unk1[4];
    bincookie_flag flags; /*!<Flags used with the cookie. */
    unsigned char unk2[4];

    time_t creation_time;   /*!< Creation time. */
    time_t expiration_time; /*!< Expiration time. */
    char *domain;           /*!< Domain name. Can be NULL. */
    char *name;             /*!< Name of the cookie. */
    char *path;             /*!< Path of the cookie. Can be NULL. */
    char *value;            /*!< Value of the cookie. */
} bincookie_cookie_t;

//! Cookie page structure. A page can consist of 1 or more cookies.
typedef struct {
    unsigned char unk1[4];             /*!< Has unknown purpose. Seems to be always `0x0 0x0 0x1 0x0`. */
    uint32_t number_of_cookies;        /*!< Number of cookies contained in this page. */
    bincookie_cookie_t **cookies; /*!< Pointer to an array of cookies. */
} bincookie_page_t;

//! bincookie file structure.
typedef struct {
    unsigned char magic[4]; /*!< Magic: "cook" */
    uint32_t num_pages; /*!< Number of pages in this file */
    uint32_t *page_sizes; /*!< Page sizes (same length as number of pages) */
    char **raw_pages;
    bincookie_page_t **pages; /*!< Array of cookie pages. */
} bincookie_t;

//! Read a bincookie file.
/*!
  \param file_path File path string.
 \return Pointer to a bincookie_t structure or `NULL`.
 */
ADDAPI bincookie_t *bincookie_init(const char *file_path);
//! Free a bincookie_t structure.
/*!
  \param cfile Pointer to data structure created with bincookie_init().
 */
ADDAPI void bincookie_free(bincookie_t *cfile);

#ifdef __cplusplus
}
#endif

#endif // _BINCOOKIE_H
