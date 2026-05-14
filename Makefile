CC = gcc
CFLAGS = -Iinclude

all: csvreader

csvreader: src/main.c src/utils.c src/parser.c src/evaluator.c src/print.c
	$(CC) $(CFLAGS) -o csvreader src/main.c src/utils.c src/parser.c src/evaluator.c src/print.c

clean:
	rm -f csvreader

