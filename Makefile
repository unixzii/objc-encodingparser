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
PROGS+=libocep.a

LIB_OBJS=$(OBJDIR)/parser.o

all: $(OBJDIR) $(PROGS)

run-test: $(LIB_OBJS) $(OBJDIR)/run-test.o
	$(CC) $(CFLAGS) -o $@ $^

libocep.a: $(LIB_OBJS)
	$(AR) -rcs $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)
	rm -rf $(PROGS)
