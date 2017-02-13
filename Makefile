P=main
OBJECTS=main.o lex.yy.o error.o entry.o dictionary.o stack.o ec_basic.o param.o globals.o
CFLAGS= -include allheads.h `pkg-config --cflags glib-2.0` -g -Wall
LDLIBS= -lfl `pkg-config --libs gsl glib-2.0`
CC=c99

%.o:%.h

$(P): $(OBJECTS)

lex.yy.c: forth.flex
	flex $<

clean:
	rm -f lex.yy.* *.o main

doc:
	doxygen doxygen.config
