# nest into whatever dir we are taking notes in and working on - universal template
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
TARGET = main
# change source for whatever you want to compile, generalized for ic.c files  (in class .c code)
SRCS = ic.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)