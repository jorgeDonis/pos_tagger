.PHONY= clean

CC=g++
OPTIONS= -g -Wall
DEBUG= #-D DEBUG
LIBDIR=lib
INCLUDEDIR=include
_OBJ= tokenizador.o tagger.o mapa_memoria.o lattice.o serializador.o

OBJ = $(patsubst %,$(LIBDIR)/%,$(_OBJ))

all: tagger
tagger: src/main.cpp $(OBJ)
	$(CC) $(OPTIONS) $(DEBUG) -I$(INCLUDEDIR) src/main.cpp $(OBJ) -o tagger

$(LIBDIR)/%.o : $(LIBDIR)/%.cpp $(INCLUDEDIR)/%.h
	$(CC) $(OPTIONS) $(DEBUG) -c -I$(INCLUDEDIR) -o $@ $<
clean:
	rm -f $(OBJ)
