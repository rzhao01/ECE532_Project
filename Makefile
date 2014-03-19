CC=gcc
LIBS=-lSDL -lSDL_gfx

CFLAGS=-Wall -g -O2 -std=c99 -D_XOPEN_SOURCE
LDFLAGS=

EXE1=connect5
EXE1_SRC=main.c
EXE2=experiment
EXE2_SRC=experiment.c

SRC=graphics.c board.c ai.c
OBJ=$(SRC:.c=.o)
EXE=$(EXE1) $(EXE2)

all:$(SRC) $(EXE)

$(EXE1): $(OBJ) $(EXE1_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(EXE1_SRC) $(LIBS) -o $@

$(EXE2): $(OBJ) $(EXE2_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(EXE2_SRC) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXE) *.o

.PHONY: all clean
