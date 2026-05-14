CC = gcc

all: csvreader

csvreader: main.c utils.c parser.c evaluator.c print.c
	$(CC) -o csvreader main.c utils.c parser.c evaluator.c print.c

clean:
	rm -f csvreader
