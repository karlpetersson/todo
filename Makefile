OUT = todo
FLAGS = -std=c99
DEPS = $(wildcard src/*.h)
INC = src/
SRC = $(wildcard src/*.c)
CC = gcc
OBJ = $(SRC:c=o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS)

all: $(OBJ)
	$(CC) $(OBJ) -o $(OUT) -I$(INC) -lm

clean:
	rm -rf src/*.o todo

install:
	cp $(OUT) /usr/local/bin
	cp todoStyles.json /usr/local/etc
