/*
 * Tlog tlog_stream test.
 *
 * Copyright (C) 2015 Red Hat
 *
 * This file is part of tlog.
 *
 * Tlog is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tlog is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tlog; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "tlog_stream.h"
#include "tlog_test.h"

#define SIZE    TLOG_STREAM_SIZE_MIN

enum op_type {
    OP_TYPE_NONE,
    OP_TYPE_WRITE,
    OP_TYPE_CUT,
    OP_TYPE_FLUSH,
    OP_TYPE_EMPTY,
    OP_TYPE_NUM
};

static const char*
op_type_to_str(enum op_type t)
{
    switch (t) {
        case OP_TYPE_NONE:
            return "none";
        case OP_TYPE_WRITE:
            return "write";
        case OP_TYPE_CUT:
            return "cut";
        case OP_TYPE_FLUSH:
            return "flush";
        case OP_TYPE_EMPTY:
            return "empty";
        default:
            return "<unkown>";
    }
}

struct op_data_write {
    uint8_t     buf[SIZE];
    size_t      len_in;
    size_t      len_out;
    ssize_t     meta_off;
    ssize_t     rem_off;
};

struct op_data_cut {
    ssize_t     meta_off;
};

struct op_data_flush {
    ssize_t     meta_off;
    ssize_t     rem_off;
};

struct op {
    enum op_type type;
    union {
        struct op_data_write    write;
        struct op_data_cut      cut;
        struct op_data_flush    flush;
    } data;
};

struct test {
    struct op       op_list[16];
    size_t          rem_in;
    size_t          rem_out;
    const uint8_t   txt_buf[SIZE];
    size_t          txt_len;
    const uint8_t   bin_buf[SIZE];
    size_t          bin_len;
    const uint8_t   meta_buf[SIZE];
    size_t          meta_len;
};

static bool
test(const char *n, const struct test t)
{
    bool passed = true;
    uint8_t meta_buf[SIZE] = {0,};
    struct tlog_stream stream;
    const uint8_t *buf;
    size_t len;
    uint8_t *meta_last = meta_buf;
    uint8_t *meta_next = meta_buf;
    size_t rem_last;
    size_t rem_next;
    const struct op *op;

    assert(t.rem_in <= SIZE);

    rem_next = rem_last = t.rem_in;

    if (tlog_stream_init(&stream, SIZE, '<', '[') !=
            TLOG_RC_OK) {
        fprintf(stderr, "Failed initializing the stream: %s\n",
                strerror(errno));
        exit(1);
    }

    memset(stream.txt_buf, 0, stream.size);
    memset(stream.bin_buf, 0, stream.size);

#define FAIL(_fmt, _args...) \
    do {                                                \
        fprintf(stderr, "%s: " _fmt "\n", n, ##_args);  \
        passed = false;                                 \
    } while (0)

#define FAIL_OP(_fmt, _args...) \
    do {                                                                \
        FAIL("op #%zd (%s): " _fmt,                                     \
             op - t.op_list + 1, op_type_to_str(op->type), ##_args);    \
        goto cleanup;                                                   \
    } while (0)

    for (op = t.op_list; op->type != OP_TYPE_NONE; op++) {
        switch (op->type) {
            case OP_TYPE_WRITE:
                assert(op->data.write.len_out <= op->data.write.len_in);
                buf = op->data.write.buf;
                len = op->data.write.len_in;
                tlog_stream_write(&stream, &buf, &len, &meta_next, &rem_next);
                if ((buf < op->data.write.buf) ||
                    (buf - op->data.write.buf) !=
                        (ssize_t)(op->data.write.len_in -
                                  op->data.write.len_out))
                    FAIL_OP("off %zd != %zu",
                            (buf - op->data.write.buf),
                            (op->data.write.len_in - op->data.write.len_out));
                if (len != op->data.write.len_out)
                    FAIL_OP("len %zu != %zu", len, op->data.write.len_out);
                if ((meta_next - meta_last) != op->data.write.meta_off)
                    FAIL_OP("meta_off %zd != %zd",
                            (meta_next - meta_last), op->data.write.meta_off);
                meta_last = meta_next;
                if (((ssize_t)rem_last - (ssize_t)rem_next) !=
                        op->data.write.rem_off)
                    FAIL_OP("rem_off %zd != %zd",
                            (rem_last - rem_next), op->data.write.rem_off);
                rem_last = rem_next;
                break;
            case OP_TYPE_CUT:
                tlog_stream_cut(&stream, &meta_next);
                if ((meta_next - meta_last) != op->data.cut.meta_off)
                    FAIL_OP("meta_off %zd != %zd",
                            (meta_next - meta_last),
                            op->data.cut.meta_off);
                meta_last = meta_next;
                break;
            case OP_TYPE_FLUSH:
                tlog_stream_flush(&stream, &meta_next, &rem_next);
                if ((meta_next - meta_last) != op->data.flush.meta_off)
                    FAIL_OP("meta_off %zd != %zd",
                            (meta_next - meta_last), op->data.flush.meta_off);
                meta_last = meta_next;
                if (((ssize_t)rem_last - (ssize_t)rem_next) !=
                        op->data.flush.rem_off)
                    FAIL_OP("rem_off %zd != %zd",
                            ((ssize_t)rem_last - (ssize_t)rem_next),
                            op->data.flush.rem_off);
                rem_last = rem_next;
                break;
            case OP_TYPE_EMPTY:
                tlog_stream_empty(&stream);
                break;
            default:
                fprintf(stderr, "Unknown operation type: %d\n", op->type);
                exit(1);
        }
    }

#undef FAIL_OP

    if (rem_last != t.rem_out)
        FAIL("rem %zu != %zu", rem_last, t.rem_out);
    if (stream.txt_len != t.txt_len)
        FAIL("txt_len %zu != %zu", stream.txt_len, t.txt_len);
    if (stream.bin_len != t.bin_len)
        FAIL("bin_len %zu != %zu", stream.bin_len, t.bin_len);
    if ((meta_last - meta_buf) != (ssize_t)t.meta_len)
        FAIL("meta_len %zd != %zu", (meta_last - meta_buf), t.meta_len);

#define BUF_CMP(_n, _r, _e) \
    do {                                                        \
        if (memcmp(_r, _e, SIZE) != 0) {                        \
            fprintf(stderr, "%s: " #_n "_buf mismatch:\n", n);  \
            tlog_test_diff(stderr, _r, _e, SIZE);               \
            passed = false;                                     \
        }                                                       \
    } while (0)
    BUF_CMP(txt, stream.txt_buf, t.txt_buf);
    BUF_CMP(bin, stream.bin_buf, t.bin_buf);
    BUF_CMP(meta, meta_buf, t.meta_buf);
#undef BUF_CMP

#undef FAIL
    fprintf(stderr, "%s: %s\n", n, (passed ? "PASS" : "FAIL"));

cleanup:
    tlog_stream_cleanup(&stream);
    return passed;
}

int
main(void)
{
    bool passed = true;

#define TEST(_name_token, _struct_init_args...) \
    passed = test(#_name_token, (struct test){_struct_init_args}) && passed

#define OP_WRITE(_data_init_args...) \
    {.type = OP_TYPE_WRITE, .data = {.write = {_data_init_args}}}

#define OP_CUT(_data_init_args...) \
    {.type = OP_TYPE_CUT, .data = {.cut = {_data_init_args}}}

#define OP_FLUSH(_data_init_args...) \
    {.type = OP_TYPE_FLUSH, .data = {.flush = {_data_init_args}}}

#define OP_EMPTY \
    {.type = OP_TYPE_EMPTY}

    TEST(null,          .op_list = {});
    TEST(one_char,      .op_list = {
                            OP_WRITE(.buf = "A",
                                     .len_in = 1,
                                     .rem_off = 3),
                            OP_CUT(.meta_off = 2)
                        },
                        .rem_in = 3,
                        .txt_buf = "A",
                        .txt_len = 1,
                        .meta_buf = "<1",
                        .meta_len = 2);

    return !passed;
}
