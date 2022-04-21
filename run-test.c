/*
 * Test runner for Objective-C encoding parser library
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
    if (argc > 1) {
        // Run with the given input.
        run_test(argv[1]);
        return 0;
    }
    
    // Run with a suite of tests.
    run_test("@@:");
    run_test("v@:");
    run_test("v@:{CGRect={CGPoint=dd}{CGSize=dd}}");
    run_test("{B={A=[5c]}i^v}");
    run_test("{?=c^vc");  // invalid encoding string on purpose
    run_test("{basic_string<char, std::char_traits<char>, std::allocator<char>>={__compressed_pair<std::basic_string<char>::__rep, std::allocator<char>>={__rep=(?={__long=*QQ}{__short=[23c]{?=C}}{__raw=[3Q]})}}}");
    return 0;
}
