# bincookie

[![QA](https://github.com/Tatsh/bincookie/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/bincookie/actions/workflows/qa.yml)
[![Tests](https://github.com/Tatsh/bincookie/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/bincookie/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/bincookie/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/bincookie?branch=master)
![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/bincookie)
![GitHub](https://img.shields.io/github/license/Tatsh/bincookie)
![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/bincookie/v0.1.1/master)

Apple has their own special binary cookie format, undocumented, in use on both OS X and heavily on iOS.

## How to build

There is no build process for this library on its own. Just include the header:

```c
#include <bincookie.h>
```

## Installation

### Header-only

Just copy `bincookie.h` to somewhere in your project and include it.

Package maintainers: if you do not want the example binary and do not want to have to install CMake,
you only need to place `bincookie.h` in a standard include directory such as `/usr/include`.

### CMake

Install CMake and make sure it is in `PATH`. Then in your terminal:

1. `git clone https://github.com/Tatsh/bincookie.git`
2. `cd bincookie`
3. `mkdir build`
4. `cmake -DCMAKE_INSTALL_PREFIX=<prefix> -DCMAKE_BUILD_TYPE=RelWithDebInfo`
5. `make install`

To build with the examples, add `-DWITH_EXAMPLES=ON` to step 4.

To build the documentation with Doxygen, add `-DWITH_DOCS=ON` to step 4.

To run tests, build with `DWITH_TESTS=ON` and run `ctest` after building. CMocka must be installed
to build tests.

Replace `<prefix>` with a value like `/usr/local`, `/usr/`, or `/opt/local` (MacPorts).

## Functions

Be sure to `#include <bincookie.h>` (which is installed with `make install`).

`bincookie_t *bincookie_init_path(const char *file_path)` - Initialise a `bincookie_t` type.

`bincookie_t *bincookie_init_file(FILE *fin)` - Initialise a `bincookie_t` type with an open file handle.

`bincookie_page_t *const bincookie_iter_pages(const bincookie_t *bc, bincookie_iter_state_t *const state)`

- Iterate pages of a file.

`bincookie_cookie_t *const bincookie_iter_cookies(const bincookie_page_t *page, unsigned int *cookie_index)`

- Iterate cookies contained within a page. `cookie_index` must be set to 0 when first calling this.

## Macros

`bool bincookie_is_secure(bincookie_cookie_t *cookie)` - Test if a cookie has secure bit set.

`bool bincookie_domain_access_full(bincookie_cookie_t *cookie)` - Test if a cookie can be used by all
subdomains.

`char *bincookie_domain(bincookie_cookie_t *cookie)` - Get the domain name for a cookie.

`char *bincookie_path(bincookie_cookie_t *cookie)` - Get the path for a cookie.

`char *bincookie_name(bincookie_cookie_t *cookie)` - Get the name of a cookie.

`char *bincookie_value(bincookie_cookie_t *cookie)` - Get the value of a cookie.

`double bincookie_expiration_time(bincookie_cookie_t *cookie)` - Get the expiration time of a cookie
as UNIX timestamp.

`double bincookie_creation_time(bincookie_cookie_t *cookie)` - Get the creation time of a cookie as
a UNIX timestamp.

`void bincookie_iter_state_init(bincookie_iter_state_t s)` - Use this to zero a
`bincookie_iter_state_t` structure.

## Flag enum

```c
typedef enum {
    secure,
    http_only,
} bincookie_flag;
```

The value that stores whether or not a cookie is secure, HTTP-only, or any of these combinations is
a single 32-bit integer, with 0 or more values OR'd together (`0` is the default value). To test for
a particular property, such as HTTP-only, use the `&` operator: `cookie->flags & http_only`.

## Example use

## Access all the cookie names in a file

```c
bincookie_t *bc = bincookie_init_path("Cookies.binarycookies");
unsigned int i, j;
bincookie_page_t *page;
bincookie_cookie_t *cookie;
bincookie_iter_state_t state;
unsigned int cookie_index = 0;

bincookie_iter_state_init(state);

while ((page = bincookie_iter_pages(bc, &state)) != NULL) {
    while ((cookie = bincookie_iter_cookies(page, &cookie_index)) != NULL) {
        printf("Name: %s\n", bincookie_name(cookie));
    }
    cookie_index = 0; // yes this is necessary
}
```
