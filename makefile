CC_LINUX = gcc
CC_WINDOWS = x86_64-w64-mingw32-gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -g

SRC = src/main.c \
      src/game.c \
      src/engine.c \
      src/uci.c

LINUX_TARGET = bin/chess-linux
WINDOWS_TARGET = bin/chess.exe

.PHONY: linux windows clean rebuild

linux:
	$(CC_LINUX) $(CFLAGS) $(SRC) -o $(LINUX_TARGET)

windows:
	$(CC_WINDOWS) $(CFLAGS) $(SRC) -o $(WINDOWS_TARGET)

clean:
	rm -f $(LINUX_TARGET) $(WINDOWS_TARGET)

rebuild: clean linux windows