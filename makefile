CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -g

TARGET = bin/chess

SRC = src/main.c \
      src/game.c \
      src/engine.c \
      src/uci.c

OBJ = build/main.o \
      build/game.o \
      build/engine.o \
      build/uci.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/game.o: src/game.c
	$(CC) $(CFLAGS) -c src/game.c -o build/game.o

build/engine.o: src/engine.c
	$(CC) $(CFLAGS) -c src/engine.c -o build/engine.o

build/uci.o: src/uci.c
	$(CC) $(CFLAGS) -c src/uci.c -o build/uci.o

clean:
	rm -f build/*.o $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild