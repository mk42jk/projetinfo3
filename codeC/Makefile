CC = gcc
CFLAGS = -Wall -Wextra
SRCS = $(wildcard *.c)
TARGET = c-wire

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
