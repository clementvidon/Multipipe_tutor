NAME        := multipipe
SRCS        := multipipe.c
OBJS        := multipipe.o
CC          := clang
CFLAGS      := -Wall -Wextra -Werror
RM          := rm -f
MAKEFLAGS	:= --no-print-directory

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(NAME)

re:
	$(MAKE) clean
	$(MAKE) all

.SILENT:
.PHONY: clean re run
