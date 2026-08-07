CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

SRCS = assembler.c preprocess.c first_pass.c second_pass.c utils.c symbol_table.c extern_table.c macro_table.c
OBJS = $(SRCS:.c=.o)
TARGET = assembler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o *.am *.ob *.ent *.ext