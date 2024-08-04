#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/limits.h>

#include "bincookie.h"
#include <cmocka.h>

typedef struct {
    char path[PATH_MAX];
    char data[2048];
    long len;
    long pos;
} fake_file_t;

bool ends_with_ext_gcda(const char *s) {
    return strlen(s) > 5 && !strcmp(s + strlen(s) - 5, ".gcda");
}

bool is_gcda(FILE *restrict stream) {
    char proc_lnk[32];
    ssize_t r;
    char filename[PATH_MAX];
    int fno = fileno(stream);
    if (fno >= 0) {
        sprintf(proc_lnk, "/proc/self/fd/%d", fno);
        r = readlink(proc_lnk, filename, PATH_MAX);
        if (r < 0) {
            return false;
        }
        filename[r] = '\0';
        return ends_with_ext_gcda(filename);
    }
    return false;
}

FILE *__real_fopen(const char *restrict path, const char *restrict mode);
FILE *__wrap_fopen(const char *restrict path, const char *restrict mode) {
    if (ends_with_ext_gcda(path)) {
        return __real_fopen(path, mode);
    }
    check_expected(path);
    check_expected(mode);
    return mock_type(FILE *);
}

int __real_fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
int __wrap_fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream) {
    if (is_gcda(stream)) {
        return __real_fread(ptr, size, nitems, stream);
    }
    memcpy(ptr, ((fake_file_t *)stream)->data, size);
    check_expected(size);
    check_expected(nitems);
    check_expected(stream);
    return mock();
}

int __real_fseek(FILE *stream, long offset, int whence);
int __wrap_fseek(FILE *stream, long offset, int whence) {
    if (is_gcda(stream)) {
        return __real_fseek(stream, offset, whence);
    }
    check_expected(stream);
    check_expected(offset);
    check_expected(whence);
    return mock();
}

long __real_ftell(FILE *stream);
long __wrap_ftell(FILE *stream) {
    if (is_gcda(stream)) {
        return __real_ftell(stream);
    }
    check_expected(stream);
    return mock();
}

void __real_rewind(FILE *stream);
void __wrap_rewind(FILE *stream) {
    if (is_gcda(stream)) {
        __real_rewind(stream);
        return;
    }
    check_expected(stream);
}

int __real_fclose(FILE *stream);
int __wrap_fclose(FILE *stream) {
    if (is_gcda(stream)) {
        return __real_fclose(stream);
    }
    check_expected(stream);
    return mock();
}

void test_not_exist(void **state) {
    (void)state;
    expect_string(__wrap_fopen, path, "null");
    expect_string(__wrap_fopen, mode, "rb");
    will_return(__wrap_fopen, NULL);
    assert_null(bincookie_init_path("null"));
}

void test_bad_magic(void **state) {
    (void)state;
    char name[] = "bad_magic.binarycookies";

    fake_file_t *ff = (fake_file_t *)malloc(sizeof(fake_file_t));
    memcpy(ff->path, name, strlen(name));
    memcpy(ff->data, "badc", 4);
    ff->len = 4;
    ff->pos = 0;

    expect_string(__wrap_fopen, path, "bad_magic.binarycookies");
    expect_string(__wrap_fopen, mode, "rb");
    will_return(__wrap_fopen, ff);

    expect_value(__wrap_ftell, stream, ff);
    will_return(__wrap_ftell, 0);

    expect_value(__wrap_rewind, stream, ff);

    expect_value(__wrap_fread, size, 4);
    expect_value(__wrap_fread, nitems, 1);
    expect_value(__wrap_fread, stream, ff);
    will_return(__wrap_fread, 4);

    expect_value(__wrap_fclose, stream, ff);
    will_return(__wrap_fclose, 0);

    expect_value(__wrap_fseek, stream, ff);
    expect_value(__wrap_fseek, offset, 0);
    expect_value(__wrap_fseek, whence, SEEK_SET);
    will_return(__wrap_fseek, 0);

    assert_null(bincookie_init_path("bad_magic.binarycookies"));
    assert_int_equal(errno, EIO);
}

void test_good(void **state) {
    (void)state;

    unsigned int data_offset = 0;
    unsigned int first_cookie_offset = sizeof(bincookie_page_t) + 4;
    unsigned int one_swapped = __builtin_bswap32(1);
    unsigned int page_size_swapped = __builtin_bswap32(128);

    bincookie_page_t first_page;
    memset(&first_page, 0, sizeof(bincookie_page_t));
    first_page.unk1[0] = 't';
    first_page.unk1[1] = 'e';
    first_page.unk1[2] = 's';
    first_page.unk1[3] = 't';
    first_page.num_cookies = 1;

    bincookie_cookie_t first_cookie;
    memset(&first_cookie, 0, sizeof(bincookie_cookie_t));
    first_cookie.unk1[0] = 't';
    first_cookie.unk1[1] = 'e';
    first_cookie.unk1[2] = 's';
    first_cookie.unk1[3] = 't';

    fake_file_t *ff = (fake_file_t *)malloc(sizeof(fake_file_t));
    memset(ff, 0, sizeof(fake_file_t));
    memcpy(ff->path, "good.binarycookies", 19);
    memcpy(ff->data, "cook", 4);
    memcpy(ff->data + 4, &one_swapped, 4);       // num_pages
    memcpy(ff->data + 8, &page_size_swapped, 4); // page_sizes[]
    data_offset += sizeof(bincookie_t) + 4;
    memcpy(ff->data + data_offset, &first_page, sizeof(bincookie_page_t));
    data_offset += sizeof(bincookie_page_t);
    memcpy(ff->data + data_offset, &first_cookie_offset, sizeof(uint32_t));
    data_offset += sizeof(uint32_t);
    memcpy(ff->data + data_offset, &first_cookie, sizeof(bincookie_cookie_t));
    data_offset += sizeof(bincookie_cookie_t);
    ff->pos = 0;
    ff->len = data_offset;

    expect_value(__wrap_ftell, stream, ff);
    will_return(__wrap_ftell, 4);

    expect_value(__wrap_rewind, stream, ff);

    expect_value(__wrap_fread, size, 4);
    expect_value(__wrap_fread, nitems, 1);
    expect_value(__wrap_fread, stream, ff);
    will_return(__wrap_fread, 1);

    expect_value(__wrap_fseek, stream, ff);
    expect_value(__wrap_fseek, offset, 0);
    expect_value(__wrap_fseek, whence, SEEK_END);
    will_return(__wrap_fseek, 0);

    expect_value(__wrap_ftell, stream, ff);
    will_return(__wrap_ftell, ff->len);

    expect_value(__wrap_rewind, stream, ff);

    expect_value(__wrap_fread, size, ff->len);
    expect_value(__wrap_fread, nitems, 1);
    expect_value(__wrap_fread, stream, ff);
    will_return(__wrap_fread, 1);

    expect_value(__wrap_fseek, stream, ff);
    expect_value(__wrap_fseek, offset, 4);
    expect_value(__wrap_fseek, whence, SEEK_SET);
    will_return(__wrap_fseek, 4);

    bincookie_t *ret = bincookie_init_file((FILE *)ff);
    assert_non_null(ret);
    assert_int_equal(1, ret->num_pages);
    assert_int_equal(128, ret->page_sizes[0]);

    bincookie_iter_state_t iter_state;
    unsigned int cookie_index = 0;
    bincookie_page_t *page;
    bincookie_cookie_t *cookie;

    bincookie_iter_state_init(iter_state);
    while ((page = bincookie_iter_pages(ret, &iter_state)) != NULL) {
        assert_int_equal(12, (void *)page - (void *)ret);
        assert_int_equal(1, iter_state.page_index);
        assert_int_equal(128 + 12, iter_state.page_offset);
        assert_int_equal(1, page->num_cookies);
        assert_memory_equal("test", page->unk1, 4);
        while ((cookie = bincookie_iter_cookies(page, &cookie_index)) != NULL) {
            assert_int_equal(1, cookie_index);
            assert_memory_equal("test", cookie->unk1, 4);
        }
    }
}

const struct CMUnitTest bincookie_tests[] = {
    cmocka_unit_test(test_bad_magic),
    cmocka_unit_test(test_good),
    cmocka_unit_test(test_not_exist),
};

int main(void) {
    return cmocka_run_group_tests(bincookie_tests, NULL, NULL);
}
