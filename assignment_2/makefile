CC=gcc
CFLAGS = -g -Wall -o
SOURCES = shell.c listf.c calc.c

all: shell listf calc

shell: shell.c
	$(CC) -o shell shell.c

listf: listf.c
	$(CC) -o listf listf.c

calc: calc.c
	$(CC) -o calc calc.c

clean:
	rm calc listf shell
