P=plankit
OBJECTS=lex.yy.o
CFLAGS= -include allheads.h `pkg-config --cflags glib-2.0` -g -Wall -O3
LDLIBS= -lfl `pkg-config --libs gsl glib-2.0`
CC=c99

lex.yy.c: forth.flex
	flex $<

$(P): $(OBJECTS)
