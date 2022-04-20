//
//  cursor.h
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#ifndef cursor_h
#define cursor_h

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

#endif /* cursor_h */
