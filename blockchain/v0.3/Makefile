CC      ?= gcc
AR      ?= ar
ARFLAGS ?= rcs
RM      ?= rm -f

WARN    := -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations
INCS    := -I. -Itransaction -I../../crypto
LIBDIRS := -L../../crypto
LIBS    := -lhblk_crypto -lllist -lssl -lcrypto

SRCS    := $(wildcard *.c) $(wildcard transaction/*.c)
OBJS    := $(patsubst %.c,%.o,$(notdir $(SRCS)))

vpath %.c . transaction

.PHONY: all clean
all: libhblk_blockchain.a clean

%.o: %.c
	@$(CC) $(WARN) -c $(INCS) $(LIBDIRS) $(LIBS) $< -o $@

libhblk_blockchain.a: $(OBJS)
	@$(AR) $(ARFLAGS) $@ $^

clean:
	@$(RM) *.o
