CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g

all:
	$(CC) $(CFLAGS) -o minimake main.c parsing.c exec.c

clean:
	rm -f minimake
