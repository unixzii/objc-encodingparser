//
//  strings.h
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#ifndef strings_h
#define strings_h

#include <stdlib.h>
#include <stddef.h>

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
    buf.str = calloc(cap, 1);
    buf.len = len;
    buf.cap = cap;
    
    if (str) {
        strcpy(buf.str, str);
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
        strcat(buf->str, str);
        buf->len += str_len;
        return;
    }
    
    char *old_str = buf->str;
    size_t new_cap = buf->cap + str_len + 1;
    char *new_str = calloc(new_cap, 1);
    if (old_str) {
        strcpy(new_str, old_str);
        free(old_str);
    }
    strcat(new_str, str);
    
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

#endif /* strings_h */
