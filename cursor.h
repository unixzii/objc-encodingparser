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

#ifndef CURSOE_H
#define CURSOE_H

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "common.h"

struct ocep_cursor {
    size_t len;
    size_t idx;
    char str[1];
};

typedef struct ocep_cursor *ocep_cursor_t;

static ocep_cursor_t ocep_cursor_new(const char *str) {
    size_t len = strlen(str);
    size_t size = sizeof(struct ocep_cursor) + len;
    
    ocep_cursor_t cursor = (ocep_cursor_t) malloc(size);
    if (!cursor) {
        return NULL;
    }
    
    cursor->len = len;
    cursor->idx = 0;
    memcpy(cursor->str, str, len);
    
    return cursor;
}

static ALWAYS_INLINE void ocep_cursor_free(ocep_cursor_t cursor) {
    free(cursor);
}


static ALWAYS_INLINE int ocep_cursor_is_eof(ocep_cursor_t cursor) {
    return cursor->idx >= cursor->len;
}

static char ocep_cursor_peek(ocep_cursor_t cursor) {
    if (ocep_cursor_is_eof(cursor)) {
        return '\0';
    }
    return cursor->str[cursor->idx];
}

static char ocep_cursor_step(ocep_cursor_t cursor) {
    if (ocep_cursor_is_eof(cursor)) {
        return '\0';
    }
    char tok = ocep_cursor_peek(cursor);
    ++cursor->idx;
    return tok;
}

#endif /* CURSOE_H */
