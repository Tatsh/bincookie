#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bincookie.h"
#include <cmocka.h>

typedef struct {
    char path[PATH_MAX];
    char data[2048];
    long len;
    long pos;
} fake_file_t;

FILE *__wrap_fopen(const char *restrict path, const char *restrict mode) {
    check_expected(path);
    check_expected(mode);
    return mock_type(FILE *);
}

int __wrap_fread(void *restrict ptr,
                 size_t size,
                 size_t nitems,
                 FILE *restrict stream) {
    memcpy(ptr, "badc", size);
    check_expected(size);
    check_expected(nitems);
    check_expected(stream);
    return mock();
}

int __wrap_fseek(FILE *stream, long offset, int whence) {
    check_expected(stream);
    check_expected(offset);
    check_expected(whence);
    return mock();
}

long __wrap_ftell(FILE *stream) {
    check_expected(stream);
    return mock();
}

void __wrap_rewind(FILE *stream) {
    check_expected(stream);
}

int __wrap_fclose(FILE *stream) {
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

    expect_value(__wrap_fseek, stream, ff);
    expect_value(__wrap_fseek, offset, 0);
    expect_value(__wrap_fseek, whence, SEEK_SET);
    will_return(__wrap_fseek, 0);

    //expect_value(__wrap_fread, ptr, &my_ptr);
    expect_value(__wrap_fread, size, 4);
    expect_value(__wrap_fread, nitems, 1);
    expect_value(__wrap_fread, stream, ff);
    will_return(__wrap_fread, 4);

    expect_value(__wrap_fclose, stream, ff);
    will_return(__wrap_fclose, 0);

    assert_null(bincookie_init_path("bad_magic.binarycookies"));
    assert_int_equal(errno, EIO);
}

const struct CMUnitTest bincookie_tests[] = {
    cmocka_unit_test(test_bad_magic),
    cmocka_unit_test(test_not_exist),
};

int main(void) {
    return cmocka_run_group_tests(bincookie_tests, NULL, NULL);
}
