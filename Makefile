#
# Makefile for holdem-poker-emulator
#
buildfiles := emulator.c input.c output.c bet.c statistic.c
checkfiles := $(buildfiles)
checkfiles += emulator.h

bin/emulator : $(checkfiles)
	gcc -o bin/emulator $(buildfiles)

clean:
	rm -f bin/emulator
