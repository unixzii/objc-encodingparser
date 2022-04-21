#
# Objective-C encoding parser library
#
# Copyright (c) 2022 Cyandev.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

ifeq ($(OS),Windows_NT)
	CONFIG_WIN32=y
else
	ifeq ($(shell uname -s),Darwin)
		CONFIG_DARWIN=y
	else
		CONFIG_POSIX=y
	endif
endif

OBJDIR=obj

ifdef CONFIG_DARWIN
	CC=clang
	CFLAGS=-g -Wall -MMD -MF $(OBJDIR)/$(@F).d
	CFLAGS += -Wextra
	CFLAGS += -Wno-sign-compare
	CFLAGS += -Wno-missing-field-initializers
	CFLAGS += -Wundef -Wuninitialized
	CFLAGS += -Wunused -Wno-unused-parameter
	CFLAGS += -Wwrite-strings
	CFLAGS += -Wchar-subscripts -funsigned-char

	AR=ar
else ifdef CONFIG_POSIX
	CC=gcc
	CFLAGS=-g -Wall

	AR=ar
endif

PROGS=run-test

LIB_OBJS=$(OBJDIR)/parser.o

all: $(PROGS) libocep.a

run-test: $(OBJDIR)/run-test.o libocep.a
	$(CC) $(CFLAGS) -o $@ $(filter-out libocep.a,$^) -L. -locep

libocep.a: $(LIB_OBJS)
	$(AR) -rcs $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)
	rm -rf $(PROGS)

-include $(wildcard $(OBJDIR)/*.d)
