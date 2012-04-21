CC?=gcc
DEBUG_CFLAGS=-O0 -ggdb3 -march=native
OPT_CFLAGS=-O2 -fomit-frame-pointer -march=native

SRCS=kindle_tool.c create.c convert.c
INCLUDES=-Llib -Iincludes
LIBS=-lcrypto -ltar -lz

ifeq "$(DEBUG)" "true"
	OUT_DIR=Debug
	CFLAGS=$(DEBUG_CFLAGS)
else
	OUT_DIR=Release
	CFLAGS=$(OPT_CFLAGS)
endif

CFLAGS+=-pipe -Wall
LDFLAGS?=

DEPS=kindle_tool.h

OBJS:=$(SRCS:%.c=$(OUT_DIR)/%.o)

$(OUT_DIR)/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(OUT_DIR)
	$(MAKE) kindletool

kindletool: $(OBJS)
	$(CC) $(INCLUDES) $(CFLAGS) $(OBJS) $(LIBS) -o$(OUT_DIR)/$@

debug:
	$(MAKE) all DEBUG=true

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/kindletool
	rm -rf Debug/*.o
	rm -rf Debug/kindletool