// Based on the Python script by Satishb3 <satishb3@securitylearn.net>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bincookie.h"

// https://msdn.microsoft.com/en-us/library/a3140177.aspx
#if defined(_MSC_VER)
#define __builtin_bswap32 _byteswap_ulong
#define EXPORT
#else
#define EXPORT __attribute__((visibility("default")))
#endif // _MSC_VER

EXPORT
bincookie_t *const bincookie_init(const char *file_path) {
    char magic[4];
    size_t read;

    FILE *binary_file = fopen(file_path, "rb");

    if (!binary_file) {
        return NULL;
    }

    read = fread(magic, sizeof(magic), 1, binary_file);
    assert(read == 1);

    if (strncmp(magic, "cook", sizeof(magic))) {
        errno = EIO;
        fclose(binary_file);
        return NULL;
    }

    // Read entire file
    fseek(binary_file, 0, SEEK_END);
    size_t num_bytes = (size_t)ftell(binary_file);
    bincookie_t *cook = malloc(num_bytes);
    memset(cook, 0, num_bytes);
    assert(cook != NULL);
    rewind(binary_file);
    fread(cook, 1, num_bytes, binary_file);
    cook->num_pages = __builtin_bswap32(cook->num_pages);
    fclose(binary_file);

    // Fix page size numbers
    for (uint32_t i = 0; i < cook->num_pages; i++) {
        *(cook->page_sizes + i) = __builtin_bswap32(*(cook->page_sizes + i));
    }

    return cook;
}

EXPORT
bincookie_page_t *const
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

EXPORT
bincookie_cookie_t *const bincookie_iter_cookies(const bincookie_page_t *page,
                                                 unsigned int *cookie_index) {
    if (*cookie_index < page->num_cookies) {
        bincookie_cookie_t *ptr =
            (bincookie_cookie_t *)((char *)page +
                                   page->cookie_offsets[*cookie_index]);
        (*cookie_index)++;
        return ptr;
    }
    return NULL;
}
