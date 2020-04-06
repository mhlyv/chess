NAME = chess

PKGS = sdl2 libpng
LIBS = $(shell pkg-config --libs $(PKGS)) -lm
CFLAGS = -Wall -Wextra -pedantic $(shell pkg-config --cflags $(PKGS))

SRC=$(wildcard *.c) $(wildcard */*.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

$(filter %.o, $(OBJ)): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: status
	cp -f $(NAME) /usr/local/bin/

execute: $(NAME)
	./$(NAME)

clean:
	rm -f $(OBJ) status
