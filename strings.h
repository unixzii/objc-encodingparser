/*
 * Objective-C encoding parser library
 *
 * Copyright (c) 2022 Cyandev.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef STRINGS_H
#define STRINGS_H

#include <stdlib.h>
#include <stddef.h>

#include "common.h"

typedef struct _strbuf {
    char *str;
    size_t len;
    size_t cap;
} strbuf;

static strbuf strbuf_alloc(const char *str, size_t extra_bytes) {
    size_t len = 0;
    if (str) {
        len = strlen(str);
    }
    size_t cap = len + extra_bytes + 1;
    
    strbuf buf;
    buf.str = (char *) calloc(cap, 1);
    buf.len = len;
    buf.cap = cap;
    
    if (str) {
#if defined(CONFIG_WIN32)
        strcpy_s(buf.str, buf.cap, str);
#else
        strcpy(buf.str, str);
#endif
    }
    
    return buf;
}

static char *strbuf_take(strbuf *buf) {
    char *str = buf->str;
    buf->str = NULL;
    buf->len = 0;
    buf->cap = 0;
    return str;
}

static UNUSED void strbuf_free(strbuf *buf) {
    char *str = strbuf_take(buf);
    if (str) {
        free(str);
    }
}

static void strbuf_append(strbuf *buf, const char *str) {
    size_t str_len = strlen(str);
    
    if (buf->len + str_len <= buf->cap - 1) {
#if defined(CONFIG_WIN32)
        strcat_s(buf->str, buf->cap, str);
#else
        strcat(buf->str, str);
#endif
        buf->len += str_len;
        return;
    }
    
    char *old_str = buf->str;
    size_t new_cap = buf->cap + str_len + 1;
    char *new_str = (char *) calloc(new_cap, 1);
    if (old_str) {
#if defined(CONFIG_WIN32)
        strcpy_s(new_str, new_cap, old_str);
#else
        strcpy(new_str, old_str);
#endif
        free(old_str);
    }
#if defined(CONFIG_WIN32)
    strcat_s(new_str, new_cap, str);
#else
    strcat(new_str, str);
#endif
    
    buf->str = new_str;
    buf->len += str_len;
    buf->cap = new_cap;
}

static void strbuf_append_repeat(strbuf *buf, char ch, int count) {
    char str[2] = { ch, 0 };
    while (count--) {
        strbuf_append(buf, str);
    }
}

#endif /* STRINGS_H */
