//
//  run-test.c
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

static void run_test(const char *test) {
    ocep_type_node_t root = ocep_parse_encoding(test);
    char *dump_str = ocep_type_node_dump(root);
    printf("--- %s ---\n%s\n\n", test, dump_str);
    free(dump_str);
    ocep_type_node_free(root);
}

int main(int argc, char **argv) {
    run_test("@@:");
    run_test("v@:");
    run_test("v@:{CGRect={CGPoint=dd}{CGSize=dd}}");
    run_test("{B={A=[5c]}i^v}");
    run_test("{?=c^vc");  // invalid encoding string on purpose
    run_test("{basic_string<char, std::char_traits<char>, std::allocator<char>>={__compressed_pair<std::basic_string<char>::__rep, std::allocator<char>>={__rep=(?={__long=*QQ}{__short=[23c]{?=C}}{__raw=[3Q]})}}}");
    return 0;
}
