CC = gcc
CFLAGS = -Wall -ansi -pedantic

turn_on: assembler.c
	$(CC) $(CFLAGS) *.c -o assembler

clean:
	rm -f assembler *.o *.am *.ob *.ent *.ext