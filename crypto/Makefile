CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -I.
NAME    := libhblk_crypto.a

SRCS    := sha256.c \
           ec_create.c \
           ec_to_pub.c \
           ec_from_pub.c \
           ec_save.c \
           ec_load.c \
           ec_sign.c \
           ec_verify.c

OBJS    := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)		# archive into library
	ar rcs $@ $^

%.o: %.c hblk_crypto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

.PHONY: all clean fclean
