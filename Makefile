CC=gcc
LIBS=-lSDL -lSDL_gfx

CFLAGS=-Wall -g -O2 -std=c99 -funroll-loops
LDFLAGS=

SRC=main.c graphics.c board.c ai.c
OBJ=$(SRC:.c=.o)
EXE=connect5

all:$(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXE) $(OBJ)

.PHONY: all clean
