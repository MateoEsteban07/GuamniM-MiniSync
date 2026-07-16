# Compilador a usar
CC = gcc

# Banderas de compilación e include path para cabeceras/
CFLAGS = -Wall -Icabeceras

TARGET = bin/minisync
SRCS = $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -pthread

clean:
	rm -f $(TARGET)