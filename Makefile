CC=gcc
LD=gcc
SRCS := ./mMemoryMgr.c ./testcase.c ./rbtree.c
#SRCS := ./mMemoryMgr.c ./testcase1.c ./rbtree.c
CFLAGS := -I./


OBJS := $(patsubst %.c, build/%.o, $(SRCS))


all: test


build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJS)
	$(LD) $(CFLAGS) $(OBJS) -o build/$@

clean:
	@rm -r build

.PHONY: all clean