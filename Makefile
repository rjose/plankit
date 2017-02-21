P=main
OBJECTS=main.o lex.yy.o entry.o dictionary.o stack.o return_stack.o ec_basic.o\
        param.o globals.o ext_notes.o ext_sqlite.o
CFLAGS= -include allheads.h `pkg-config --cflags glib-2.0 sqlite3` -g -Wall
LDLIBS= -L. -ljsmn -lfl `pkg-config --libs gsl glib-2.0 sqlite3`
CC=c99

%.o:%.h

$(P): $(OBJECTS)

lex.yy.c: forth.flex
	flex $<

clean:
	rm -f lex.yy.* *.o main

doc:
	doxygen doxygen.config
