CC := gcc
CFLAGS := -g -Wall -Wextra

main: main.o base.o think.o

main.o base.o: reversi.h
think.o: reversi.h graphtree.h

clean:
	rm main
	rm main.o base.o think.o
	rm *.dot
