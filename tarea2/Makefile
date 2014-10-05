GCC = gcc 
EXECUTABLE = csa

#Se agrega al VPATH las siguientes para facilitar el uso de varias carpetas dejando mas ordenado el programa.
VPATH = ./src ./include ./objects
CFLAGS = -Wall -fPIC -I./include -I./objects -I./src

all: csa 

csa: csa.c
	mkdir -p bin
	$(GCC) $(CFLAGS) ./src/csa.c -o ./bin/$(EXECUTABLE)
clean:
	rm -Rf ./bin
	rm -Rf ./objects
