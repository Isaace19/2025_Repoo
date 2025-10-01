CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
TARGET = main
SRCS = main.c pagealloc_another.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)