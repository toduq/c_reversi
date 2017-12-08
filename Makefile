CC := gcc
CFLAGS := -g -Wall -Wextra

main: main.o base.o think.o

main.o base.o think.o: reversi.h

clean:
	rm main
	rm main.o base.o think.o
