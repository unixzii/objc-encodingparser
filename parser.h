//
//  parser.h
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#ifndef parser_h
#define parser_h

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

#endif /* parser_h */
