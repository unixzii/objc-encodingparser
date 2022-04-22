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

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
#define __OBJC_TYPE(name, value, _0, _1, _2) name = value,
#include "types.def"
#undef __OBJC_TYPE
};

typedef struct ocep_type_node *ocep_type_node_t;

struct ocep_type_node {
    /* Objective-C Type, see `types.def` for enum values.
       Return 0 for root nodes. */
    int type;
    
    /* Number of child types (for array, struct and union types).
       Note that there will only be one node in the child list
       for array type. */
    int child_num;
    /* Size of the value of this type. */
    int size;
    /* Alignment of this type. */
    int alignment;
    
    /* Struct name. Only exists for struct types. */
    const char *name;
    
    /* Parent node that holds this node. */
    ocep_type_node_t parent;
    /* First child node, NULL if the type contains no children. */
    ocep_type_node_t first_child;
    /* Next sibling node, NULL if this is the last node of the
       child list. */
    ocep_type_node_t next_sibling;
};

/* Parses the given encoding string and returns the type node.
   The caller must call `ocep_type_node_free` to free the memory
   when it has done with it. */
ocep_type_node_t ocep_parse_encoding(const char *encoding);

/* Deallocates the node object. */
void ocep_type_node_free(ocep_type_node_t node);

/* Recursively visits the given node, and invokes the given
   callback with each child node visited. */
void ocep_type_node_traverse(ocep_type_node_t node, void *userdata,
                             void (*cb)(ocep_type_node_t cur, int num, int depth, void *userdata));

/* Dumps the given node to human-readable string. The caller
   must free the result string when it has done with it. */
char *ocep_type_node_dump(ocep_type_node_t node);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* PARSER_H */
