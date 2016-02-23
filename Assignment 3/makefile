CC=gcc -g3
CFLAGS = -g -w -o

all: FileSys


FileSys: FileSysAPI.c
	gcc -g -w FileSysAPI.c Driver.o EasyTest.c -o FileSysAPI

clean:
	rm FileSysAPI
