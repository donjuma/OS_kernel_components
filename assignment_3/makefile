CC = gcc
CFLAGS = -g `pkg-config --cflags --libs gtk+-3.0`
LFLAGS = -lm

draw:	draw.c
	$(CC) draw.c -o draw $(CFLAGS) $(LFLAGS)


main: main.c
	$(CC) main.c cpu.c -o main $(CFLAGS) $(LFLAGS)


clean:
	rm -f draw tree *.o
