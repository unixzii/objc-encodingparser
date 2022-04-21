//
//  parser.c
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "cursor.h"
#include "strings.h"
#include "common.h"

static ocep_type_node_t ocep_type_node_new() {
    ocep_type_node_t node = (ocep_type_node_t) malloc(sizeof(struct ocep_type_node));
    node->type = OBJC_TYPE_UNKNOWN;
    node->child_num = 0;
    node->size = 0;
    node->alignment = 0;
    node->name = NULL;
    node->parent = NULL;
    node->first_child = NULL;
    node->next_sibling = NULL;
    return node;
}

static void ocep_type_node_free_internal(ocep_type_node_t node) {
    if (node->name) {
        free((void *) node->name);
    }
    free(node);
}

static NO_INLINE int _parse_number(ocep_cursor_t cursor) {
    int num = 0;
    char tok;
    while (!ocep_cursor_is_eof(cursor)) {
        tok = ocep_cursor_peek(cursor);
        if (tok < '0' || tok > '9') {
            return num;
        }
        num = num * 10 + (tok - '0');
        ocep_cursor_step(cursor);
    }
    return num;
}

static ALWAYS_INLINE void _skip_digits(ocep_cursor_t cursor) {
    (void) _parse_number(cursor);
}

static ocep_type_node_t _parse_one_element(ocep_cursor_t cursor, ocep_type_node_t parent);

static void _parse_struct_or_union(int type, ocep_cursor_t cursor, ocep_type_node_t node) {
    // Structs are represented as "{name=??}", the cursor has moved pass '{'
    // when this function is called.
    
    // Collect struct name.
    strbuf name_buf = strbuf_alloc("", 8);
    while (!ocep_cursor_is_eof(cursor)) {
        char tok_str[2] = { 0, 0 };
        tok_str[0] = ocep_cursor_step(cursor);
        if (tok_str[0] == '=') {
            break;
        }
        strbuf_append(&name_buf, tok_str);
    }
    
    char close_tok;
    if (type == OBJC_TYPE_STRUCT) {
        close_tok = '}';
    } else {
        close_tok = ')';
    }
    
    // Parse child nodes.
    ocep_type_node_t last_child = NULL;
    while (!ocep_cursor_is_eof(cursor)) {
        char tok = ocep_cursor_peek(cursor);
        if (tok == close_tok) {
            ocep_cursor_step(cursor);
            break;
        }
        
        ocep_type_node_t child = _parse_one_element(cursor, node);
        if (last_child) {
            last_child->next_sibling = child;
        } else {
            node->first_child = child;
        }
        last_child = child;
        
        node->alignment = MAX(node->alignment, child->alignment);
        if (type == OBJC_TYPE_STRUCT) {
            // FIXME: current naive algorithm assumes the compact layout.
            node->size += child->size;
        } else {
            node->size = MAX(node->size, child->size);
        }
        ++node->child_num;
    }
    
    if (type == OBJC_TYPE_STRUCT) {
        node->size = ROUNDUP(node->size, node->alignment);
    }
    
    node->type = type;
    node->name = strbuf_take(&name_buf);
}

static void _parse_array(ocep_cursor_t cursor, ocep_type_node_t node) {
    int count = _parse_number(cursor);
    
    ocep_type_node_t child = _parse_one_element(cursor, node);
    node->first_child = child;
    node->type = OBJC_TYPE_ARRAY;
    node->child_num = count;
    node->size = child->size * count;
    node->alignment = child->alignment;
    
    char tok = ocep_cursor_peek(cursor);
    if (tok == ']') {
        ocep_cursor_step(cursor);
    }
}

static void _parse_pointer(ocep_cursor_t cursor, ocep_type_node_t node) {
    ocep_type_node_t child = _parse_one_element(cursor, node);
    node->first_child = child;
    node->type = OBJC_TYPE_POINTER;
    node->child_num = 1;
    node->size = 8;
    node->alignment = 8;
}

static ocep_type_node_t _parse_one_element(ocep_cursor_t cursor, ocep_type_node_t parent) {
    ocep_type_node_t node = ocep_type_node_new();
    node->parent = parent;
    
    char tok = ocep_cursor_step(cursor);
    switch (tok) {
#define __NO_COMPLEX_OBJC_TYPES
#define __OBJC_TYPE(_0, val, rep, siz, _1) \
        case rep:                          \
            node->type = val;              \
            node->size = siz;              \
            node->alignment = siz;         \
            break;
#include "types.def"
#undef __OBJC_TYPE
#undef __NO_COMPLEX_OBJC_TYPES
        case '{':
            _parse_struct_or_union(OBJC_TYPE_STRUCT, cursor, node);
            break;
        case '(':
            _parse_struct_or_union(OBJC_TYPE_UNION, cursor, node);
            break;
        case '[':
            _parse_array(cursor, node);
            break;
        case '^':
            _parse_pointer(cursor, node);
            break;
    }
    
    // Skip the frame sizes and offsets, if any.
    _skip_digits(cursor);
    
    return node;
}

ocep_type_node_t ocep_parse_encoding(const char *encoding) {
    ocep_cursor_t cursor = ocep_cursor_new(encoding);
    
    // Allocate the root node.
    ocep_type_node_t root = ocep_type_node_new();
    root->type = 0;
    
    // Consume all tokens and generate subnodes.
    ocep_type_node_t last_elem = NULL;
    while (!ocep_cursor_is_eof(cursor)) {
        ocep_type_node_t elem = _parse_one_element(cursor, root);
        if (last_elem) {
            last_elem->next_sibling = elem;
        } else {
            root->first_child = elem;
        }
        last_elem = elem;
        ++root->child_num;
    }
    
    ocep_cursor_free(cursor);
    return root;
}

void ocep_type_node_free(ocep_type_node_t node) {
    if (node->parent) {
        abort();  // Child nodes must not be freed via `ocep_type_node_free`.
        return;
    }
    
    // Non-recursive free with node links.
    ocep_type_node_t cur = node;
    while (cur) {
        ocep_type_node_t to_free = NULL;
        if (cur->first_child) {
            ocep_type_node_t next = cur->first_child;
            cur->first_child = NULL;
            cur = next;
        } else if (cur->next_sibling) {
            to_free = cur;
            cur = cur->next_sibling;
        } else {
            to_free = cur;
            cur = cur->parent;
        }
        
        if (to_free)
            ocep_type_node_free_internal(to_free);
    }
}

static void ocep_type_node_traverse_internal(ocep_type_node_t node, void *userdata,
                                             int *num, int depth,
                                             void (*cb)(ocep_type_node_t cur, int num, int depth, void *userdata)) {
normal:
    cb(node, *num, depth, userdata);
    ++*num;
    
    // Visit children.
    ocep_type_node_t first_child = node->first_child;
    if (first_child) {
        ocep_type_node_traverse_internal(first_child, userdata, num, depth + 1, cb);
    }
    
    // Then visit rest siblings.
    ocep_type_node_t next_sibling = node->next_sibling;
    if (next_sibling) {
        node = next_sibling;
        goto normal;  // Force tail-recursion.
    }
}

void ocep_type_node_traverse(ocep_type_node_t node, void *userdata,
                             void (*cb)(ocep_type_node_t cur, int num, int depth, void *userdata)) {
    int num = 0;
    ocep_type_node_traverse_internal(node, userdata, &num, 0, cb);
}

struct __ocep_type_node_dump_state {
    strbuf str;
};

// Outlined common routine for switch cases in `ocep_type_node_dump_visitor`.
static NO_INLINE void ocep_type_node_dump_outlined_1(strbuf *str_buf,
                                                     int depth, const char *type, char encoding) {
    strbuf_append_repeat(str_buf, ' ', depth * 4);
    strbuf_append(str_buf, "type:      ");
    strbuf_append(str_buf, type);
    strbuf_append(str_buf, "\n");
    
    strbuf_append_repeat(str_buf, ' ', depth * 4);
    strbuf_append(str_buf, "encoding:  ");
    char encoding_str[2] = { encoding, 0 };
    strbuf_append(str_buf, encoding_str);
    strbuf_append(str_buf, "\n");
}

static NO_INLINE void ocep_type_node_dump_outlined_2(strbuf *str_buf,
                                                     int depth, const char *label, int value) {
    strbuf_append_repeat(str_buf, ' ', depth * 4);
    strbuf_append(str_buf, label);
    char value_str[32];
#if defined(CONFIG_WIN32)
    sprintf_s(value_str, sizeof(value_str), "%d", value);
#else
    sprintf(value_str, "%d", value);
#endif
    strbuf_append(str_buf, value_str);
    strbuf_append(str_buf, "\n");
}

static void ocep_type_node_dump_visitor(ocep_type_node_t node, int num, int depth, void *userdata) {
    if (node->type == 0) {
        return;
    }
    
    --depth;  // Root node is skipped.
    
    struct __ocep_type_node_dump_state* state;
    state = (struct __ocep_type_node_dump_state*) userdata;
    
    strbuf *str_buf = &state->str;
    
    // Draw a separator line.
    if (num > 1) {
        strbuf_append_repeat(str_buf, ' ', depth * 4);
        strbuf_append_repeat(str_buf, '-', 30);
        strbuf_append(str_buf, "\n");
    }
    
    switch (node->type) {
#define __OBJC_TYPE(name, val, rep, _0, _1)                             \
        case val:                                                       \
            ocep_type_node_dump_outlined_1(str_buf, depth, #name, rep); \
            break;
#include "types.def"
#undef __OBJC_TYPE
    }
    
    ocep_type_node_dump_outlined_2(str_buf, depth, "size:      ", node->size);
    ocep_type_node_dump_outlined_2(str_buf, depth, "alignment: ", node->alignment);
    if (node->child_num > 1) {
        ocep_type_node_dump_outlined_2(str_buf, depth, "child num: ", node->child_num);
    }
    
    const char *name = node->name;
    if (name) {
        strbuf_append_repeat(str_buf, ' ', depth * 4);
        strbuf_append(str_buf, "name:     ");
        strbuf_append(str_buf, name);
        strbuf_append(str_buf, "\n");
    }
}

char *ocep_type_node_dump(ocep_type_node_t node) {
    struct __ocep_type_node_dump_state state;
    state.str = strbuf_alloc("", 128);
    ocep_type_node_traverse(node, &state, &ocep_type_node_dump_visitor);
    return strbuf_take(&state.str);
}
