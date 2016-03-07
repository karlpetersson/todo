OUT = todo
FLAGS = -Wall
INC = src/
SRC = $(wildcard src/*.c)
CC = gcc
OBJ = $(SRC:c=o)

$(OUT): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(OUT) -I$(INC)

all: $(OUT)

clean: 
	rm -rf src/*.o todo