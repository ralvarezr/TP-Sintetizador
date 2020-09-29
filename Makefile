PROGRAM=sintetizador
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -g
LDFLAGS=-lm

all: $(PROGRAM)

$(PROGRAM): main.o midi.o nota.o contenedor.o tramo.o sintetizador.o wav.o
	$(CC) $(CFLAGS) main.o midi.o nota.o contenedor.o sintetizador.o tramo.o wav.o -o $(PROGRAM) $(LDFLAGS)

main.o: main.c nota.h midi.h  contenedor.h sintetizador.h tramo.h wav.h
	$(CC) $(CFLAGS) -c main.c

nota.o: nota.c nota.h
	$(CC) $(CFLAGS) -c nota.c

midi.o: midi.c midi.h nota.h
	$(CC) $(CFLAGS) -c midi.c

contenedor.o: contenedor.c contenedor.h nota.h
	$(CC) $(CFLAGS) -c contenedor.c

tramo.o: tramo.c tramo.h sintetizador.h
	$(CC) $(CFLAGS) -c tramo.c $(LDFLAGS)

sintetizador.o: sintetizador.c sintetizador.h contenedor.h tramo.h nota.h
	$(CC) $(CFLAGS) -c sintetizador.c $(LDFLAGS)

wav.o: wav.c wav.h tramo.h
	$(CC) $(CFLAGS) -c wav.c


clean:
	rm -vf *.o $(PROGRAM)
