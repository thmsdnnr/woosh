CC	= gcc
CFLAGS	+= -I. -Wall -Wextra -Werror -std=c99
DEBUGFLAGS +=  $(CFLAGS) -g -O

NAME = "woosh"
DEBUGNAME = "woosh-d"

FILES = woosh.c main.c
VALGRINDFLAGS = --leak-check=full --tool=memcheck --show-leak-kinds=all -s

TEST_SCRIPT = test-script-exit0

all:
	$(CC) -o $(NAME) $(FILES) $(CFLAGS)

debug:
	$(CC) -o $(DEBUGNAME) $(FILES) $(DEBUGFLAGS)

leakcheck:
	$(MAKE) debug
	valgrind $(VALGRINDFLAGS) ./$(DEBUGNAME) ./$(TEST_SCRIPT)

clean:
	rm -f $(NAME) $(DEBUGNAME)
