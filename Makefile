CC = gcc
CFLAGS = -Wall 


SRCS = main.c parsing.c
OBJS = $(SRCS:.c=.o)


TARGET = minimake


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o 


%.o: %.c
	$(CC) $(CFLAGS) -c 

clean:
	rm -f $(TARGET) $(OBJS)
