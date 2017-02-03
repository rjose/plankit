P=plankit
OBJECTS=
CFLAGS= -include allheads.h -g -Wall -O3
LDLIBS= `pkg-config --libs gsl`
CC=c99

$(P): $(OBJECTS)
