# General

Apple has their own special binary cookie format, undocumented, in use on both OS X and heavily on iOS.

# How to build

You can easily include the code statically with your app or you can build a library.

## Shared library

Install CMake and make sure it is in `PATH`. Then in your terminal:

1. `git clone https://github.com/Tatsh/bincookie.git`
2. `cd bincookie`
3. `mkdir build`
4. `cmake -DCMAKE_INSTALL_PREFIX=<prefix> -DCMAKE_BUILD_TYPE=RelWithDebugInfo`
5. `make install`

To build with the examples, add `-DWITH_EXAMPLES=yes` to step 4.

Replace `<prefix>` with a value like `/usr/local`, `/usr/`, or `/opt/local` (MacPorts).

# Functions

Be sure to `#include <bincookie.h>` (which is installed with `make install`).

`bincookie_t *bincookie_init(const char *file_path)` - Initialise a `bincookie_t` type.

`void bincookie_free(bincookie_t *cfile)` - Free a `bincookie_t` type.

# Macros

`bool bincookie_is_secure(bincookie_cookie_t *cookie)` - Test if a cookie has secure bit set.

`bool bincookie_domain_access_full(bincookie_cookie_t *cookie)` - Test if a cookie can be used by all subdomains.

# Data structures

## File

```c
typedef struct {
    unsigned char magic[4];
    uint32_t num_pages;
    bincookie_page_t **pages;
} bincookie_t;
```

### Fields

* `magic` - the string "cook"
* `num_pages` - number of pages
* `pages` - pointer to array of cookie pages

## Cookie page

```c
typedef struct {
    uint32_t number_of_cookies;
    bincookie_cookie_t **cookies;
} bincookie_page_t;
```

### Fields

* `number_of_cookies` - number of cookies contained in this page
* `cookies` - pointer to array of cookies

## Cookie data

```c
typedef struct {
    bincookie_flag flags;
    time_t creation_date;
    time_t expiration_date;
    char *domain;
    char *name;
    char *path;
    char *value;
} bincookie_cookie_t;
```

### Fields

* `flags` - if the cookie is secure, HTTP-only, etc
* `creation_date` - UNIX timestamp of creation date
* `expiration_date` - UNIX timestamp of expiration date
* `domain` - domain value. Can be `NULL`
* `name` - name of cookie
* `path` - path the cookie is allowed to be used on. Can be `NULL`
* `value` - value of the cookie

## Flag enum

```c
typedef enum {
    secure,
    http_only,
} bincookie_flag;
```

The value that stores whether or not a cookie is secure, HTTP-only, or any of these combinations is a single 32-bit integer, with 0 or more values OR'd together (`0` is the default value). To test for a particular property, such as HTTP-only, use the `&` operator: `cookie->flags & http_only`.

# Examples

## Access all the cookie names in a file

```c
bincookie_t *bc = bincookie_init("Cookies.binarycookies");
unsigned int i, j;
bincookie_cookie_t *cookie;

for (i = 0; i < bc->num_pages; i++) {
    for (j = 0, cookie = bc->pages[i]->cookies[j];
         j < bc->pages[i]->number_of_cookies;
         j++, cookie = bc->pages[i]->cookies[j]) {
        printf("Name: %s\n", cookie->name);
    }
}
```
