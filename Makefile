P=main
OBJECTS=main.o lex.yy.o error.o entry.o dictionary.o stack.o ec_basic.o globals.o
CFLAGS= -include allheads.h `pkg-config --cflags glib-2.0` -g -Wall -O3
LDLIBS= -lfl `pkg-config --libs gsl glib-2.0`
CC=c99


$(P): $(OBJECTS)

lex.yy.c: forth.flex
	flex $<
