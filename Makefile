CC = gcc
CFLAGS = -Wall -ansi -pedantic

turn_on: assembler.c
	$(CC) $(CFLAGS) assembler.c -o assembler

clean:
	rm -f assembler *.o