# General

Apple has their own special binary cookie format, undocumented, in use on both OS X and heavily on iOS.

# How to build

1. `git clone https://github.com/Tatsh/libbinarycookies.git libbinarycookies`
2. `cd libbinarycookies`
3. `make`

# Installation

1. `make DESTDIR=some_destination install`

It is preferable that you create `some_destination` *before* running `make install`.

# Functions

`binarycookies_t *binarycookies_init(const char *file_path)` - Initialise a `binarycookies_t` type.

`void binarycookies_free(binarycookies_t *cfile)` - Free a `binarycookies_t` type.

## Examples

### Access all the cookie names in a file

```c
binarycookies_t *bc = binarycookies_init("Cookies.binarycookies");
unsigned int i, j;

for (i = 0; i < bc->num_pages; i++) {
    for (j = 0; j < bc->pages[i]->number_of_cookies; j++) {
        printf("Name: %s\n", bc->pages[i]->cookies[j]->name);
    }
}
```

# Macros

`bool binarycookies_is_secure(binarycookies_cookie_t *cookie)` - Test if a cookie must be used in a secure manner (HTTPS-only, etc).

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
```

### Fields

* `flags` - if the cookie is secure, HTTP-only, etc
* `flags_str` - helper string for the flag (example: *Secure; HttpOnly*)
* `creation_date` - UNIX timestamp of creation date
* `creation_date_str` - ISO date string of creation date
* `expiration_date` - UNIX timestamp of expiration date
* `expiration_date_str` - ISO date string of expiration date
* `domain` - domain value
* `name` - name of cookie
* `path` - path the cookie is allowed to be used on
* `value` - value of the cookie

## Flag enum

```c
typedef enum {
    secure = 1,
    http_only = 1 << 2,
    secure_http_only = 5,
} binarycookies_flag;
```

The value that stores whether or not a cookie is secure, HTTP-only, or any of these combinations is a single 32-bit integer. It is unknown if this is a series of flags (via some mechanism such as OR'ing) or if these are just defined with all combinations. Since *Secure + HTTP only* appears is not a power of 2, it seems the latter is more likely (although that seems strange).
