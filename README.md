# General

Apple has their own special binary cookie format, undocumented, in use on both OS X and heavily on iOS.

# How to build

## On OS X

1. `git clone https://github.com/Tatsh/libbinarycookies.git libbinarycookies`
2. `cd libbinarycookies`
3. `make`

### How to link with Clang

Example command line:

```bash
clang my_code.c libbinarycookies.A.dylib -o my_app
```

## Everything else

1. `git clone https://github.com/Tatsh/libbinarycookies.git libbinarycookies`
2. `cd libbinarycookies`
3. `make`

## Fun: Wine

Use `make CC=winegcc clean examples` and run `wine convert2netscape.exe.so path-to.binarycookies`.

# Installation

1. `make DESTDIR=some_destination install`

It is preferable that you create `some_destination` *before* running `make install`.

# Functions

Be sure to `#include <binarycookies.h>` (which is installed with `make install`).

`binarycookies_t *binarycookies_init(const char *file_path)` - Initialise a `binarycookies_t` type.

`void binarycookies_free(binarycookies_t *cfile)` - Free a `binarycookies_t` type.

## Examples

### Access all the cookie names in a file

```c
binarycookies_t *bc = binarycookies_init("Cookies.binarycookies");
unsigned int i, j;
binarycookies_cookie_t *cookie;

for (i = 0; i < bc->num_pages; i++) {
    for (j = 0, cookie = bc->pages[i]->cookies[j];
         j < bc->pages[i]->number_of_cookies;
         j++, cookie = bc->pages[i]->cookies[j]) {
        printf("Name: %s\n", cookie->name);
    }
}
```

# Macros

`bool binarycookies_is_secure(binarycookies_cookie_t *cookie)` - Test if a cookie has secure bit set.

`bool binarycookies_domain_access_full(binarycookies_cookie_t *cookie)` - Test if a cookie can be used by all subdomains.

# Data structures

## File

```c
typedef struct {
    unsigned char magic[4];
    uint32_t num_pages;
    binarycookies_page_t **pages;
} binarycookies_t;
```

### Fields

* `magic` - the string "cook"
* `num_pages` - number of pages
* `pages` - pointer to array of cookie pages

## Cookie page

```c
typedef struct {
    uint32_t number_of_cookies;
    binarycookies_cookie_t **cookies;
} binarycookies_page_t;
```

### Fields

* `number_of_cookies` - number of cookies contained in this page
* `cookies` - pointer to array of cookies

## Cookie data

```c
typedef struct {
    binarycookies_flag flags;
    time_t creation_date;
    time_t expiration_date;
    char *domain;
    char *name;
    char *path;
    char *value;
} binarycookies_cookie_t;
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
} binarycookies_flag;
```

The value that stores whether or not a cookie is secure, HTTP-only, or any of these combinations is a single 32-bit integer, with 0 or more values OR'd together (`0` is the default value). To test for a particular property, such as HTTP-only, use the `&` operator: `cookie->flags & http_only`.
