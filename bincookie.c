// Based on the Python script by Satishb3 <satishb3@securitylearn.net>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bincookie.h"

#define APPLE_EPOCH_OFFSET 978307200

// https://msdn.microsoft.com/en-us/library/a3140177.aspx
#if defined(_MSC_VER)
#define __builtin_bswap32 _byteswap_ulong
#define EXPORT
#else
#define EXPORT __attribute__((visibility("default")))
#endif // _MSC_VER

EXPORT
binarycookies_t *binarycookies_init(const char *file_path) {
    unsigned int i, j;
    signed int slen;
    uint32_t *cookie_offsets;
    char magic[4];
    size_t read;

    FILE *binary_file = fopen(file_path, "rb");

    if (!binary_file) {
        fprintf(stderr, "Unable to open file: %s\n", file_path);
        return NULL;
    }

    read = fread(magic, sizeof(magic), 1, binary_file);
    assert(read == 1);

    if (strncmp(magic, "cook", sizeof(magic))) {
        fprintf(stderr, "Not a correctly formatted file (incorrect magic)\n");
        return NULL;
    }

    binarycookies_t *cfile = malloc(sizeof(binarycookies_t));
    memset(cfile, 0, sizeof(binarycookies_t));
    memcpy(cfile->magic, magic, 4);

    read = fread(&cfile->num_pages, sizeof(uint32_t), 1, binary_file); // big endian
    assert(read == 1);
    cfile->num_pages = __builtin_bswap32(cfile->num_pages);
    cfile->raw_pages = malloc(sizeof(char *) * cfile->num_pages);
    memset(cfile->raw_pages, 0, sizeof(char *) * cfile->num_pages);
    cfile->pages = malloc(sizeof(binarycookies_page_t *) * cfile->num_pages);
    memset(cfile->pages, 0, sizeof(binarycookies_page_t *) * cfile->num_pages);

    // size: num_pages * sizeof(int), each page size is big endian
    cfile->page_sizes = malloc(sizeof(uint32_t) * cfile->num_pages);
    memset(cfile->page_sizes, 0, sizeof(uint32_t) * cfile->num_pages);
    for (i = 0; i < cfile->num_pages; i++) {
        uint32_t page_size;

        read = fread(&page_size, sizeof(uint32_t), 1, binary_file);
        assert(read == 1);
        cfile->page_sizes[i] = __builtin_bswap32(page_size);
    }

    for (i = 0; i < cfile->num_pages; i++) {
        char *ps = malloc(cfile->page_sizes[i]);
        memset(ps, 0, cfile->page_sizes[i]);

        read = fread(ps, cfile->page_sizes[i], 1, binary_file);
        assert(read == 1);
        cfile->raw_pages[i] = ps;
    }

    fclose(binary_file);

    // Read
    for (i = 0; i < cfile->num_pages; i++) {
        binarycookies_page_t *page = malloc(sizeof(binarycookies_page_t));
        memset(page, 0, sizeof(binarycookies_page_t));
        memcpy(page->unk1, cfile->raw_pages[i], 4);

        char *page_ptr = cfile->raw_pages[i] + 4; // skip 4 bytes, always 0x0 0x0 0x1 0x0

        memcpy(&page->number_of_cookies, page_ptr, sizeof(uint32_t));
        page_ptr += sizeof(uint32_t);

        cookie_offsets = malloc(sizeof(uint32_t) * page->number_of_cookies);
        memset(cookie_offsets, 0, sizeof(uint32_t) * page->number_of_cookies);
        page->cookies = malloc(sizeof(binarycookies_cookie_t) * page->number_of_cookies);
        memset(page->cookies, 0, sizeof(binarycookies_cookie_t) * page->number_of_cookies);
        for (j = 0; j < page->number_of_cookies; j++) {
            uint32_t cookie_offset;

            memcpy(&cookie_offset, page_ptr, sizeof(uint32_t));
            cookie_offsets[j] = cookie_offset;
            page_ptr += sizeof(uint32_t);
        }

        page_ptr += sizeof(uint32_t); // end of page header

        for (j = 0; j < page->number_of_cookies; j++) {
            page_ptr = cfile->raw_pages[i] + cookie_offsets[j];
            binarycookies_cookie_t *cookie = malloc(sizeof(binarycookies_cookie_t));
            memset(cookie, 0, sizeof(binarycookies_cookie_t));
            page->cookies[j] = cookie;

            memcpy(cookie, page_ptr, sizeof(uint32_t) + 4 + sizeof(uint32_t) + 4);
            page_ptr += sizeof(uint32_t) + 4 + sizeof(uint32_t) + 4;

            uint32_t url_offset;
            memcpy(&url_offset, page_ptr, sizeof(uint32_t));
            page_ptr += sizeof(uint32_t);

            uint32_t name_offset;
            memcpy(&name_offset, page_ptr, sizeof(uint32_t));
            page_ptr += sizeof(uint32_t);

            uint32_t path_offset;
            memcpy(&path_offset, page_ptr, sizeof(uint32_t));
            page_ptr += sizeof(uint32_t);

            uint32_t value_offset;
            memcpy(&value_offset, page_ptr, sizeof(uint32_t));
            page_ptr += sizeof(uint32_t);

            page_ptr += sizeof(uint32_t) * 2; // end of cookie

            double ce_time;
            memcpy(&ce_time, page_ptr, sizeof(double));
            cookie->expiration_time = (time_t)ce_time + APPLE_EPOCH_OFFSET;
            page_ptr += sizeof(double);

            memcpy(&ce_time, page_ptr, sizeof(double));
            cookie->creation_time = (time_t)ce_time + APPLE_EPOCH_OFFSET;
            page_ptr += sizeof(double);

            page_ptr = cfile->raw_pages[i] + cookie_offsets[j] + url_offset;
            slen = name_offset - url_offset;
            if (slen > 0) {
                cookie->domain = malloc(slen);
                memset(cookie->domain, 0, slen);
                memcpy(cookie->domain, page_ptr, slen);
            }

            page_ptr = cfile->raw_pages[i] + cookie_offsets[j] + name_offset;
            slen = path_offset - name_offset;
            if (slen > 0) {
                cookie->name = malloc(slen);
                memset(cookie->name, 0, slen);
                memcpy(cookie->name, page_ptr, slen);
            }

            page_ptr = cfile->raw_pages[i] + cookie_offsets[j] + path_offset;
            slen = value_offset - path_offset;
            if (slen > 0) {
                cookie->path = malloc(slen);
                memset(cookie->path, 0, slen);
                memcpy(cookie->path, page_ptr, slen);
            }

            page_ptr = cfile->raw_pages[i] + cookie_offsets[j] + value_offset;
            slen = 0;
            int k = 0;
            while (page_ptr[k] != 0) {
                slen += 1;
                k++;
            }
            if (slen > 0) {
                cookie->value = malloc(slen);
                memset(cookie->value, 0, slen);
                memcpy(cookie->value, page_ptr, slen);
            }
        }

        free(cookie_offsets);

        cfile->pages[i] = page;
    }

    return cfile;
}

EXPORT
void binarycookies_free(binarycookies_t *cfile) {
    unsigned int i, j;

    for (i = 0; i < cfile->num_pages; i++) {
        free(cfile->raw_pages[i]);

        for (j = 0; j < cfile->pages[i]->number_of_cookies; j++) {
            free(cfile->pages[i]->cookies[j]->domain);
            free(cfile->pages[i]->cookies[j]->name);
            free(cfile->pages[i]->cookies[j]->path);
            free(cfile->pages[i]->cookies[j]->value);
            free(cfile->pages[i]->cookies[j]);
        }
        free(cfile->pages[i]->cookies);
        free(cfile->pages[i]);
    }
    free(cfile->pages);
    free(cfile->raw_pages);
    free(cfile->page_sizes);
    free(cfile);
}
