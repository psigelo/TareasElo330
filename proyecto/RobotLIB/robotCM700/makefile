CC = g++ -O3
VPATH = ./src ./headers 
CFLAGS = -g -Wall -I./src -I./headers -I./objects -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
LDFLAGS = -lpthread

all: cm700.o serial.o

cm700.o: cm700.cpp
	@mkdir -p objects
	@echo "Compiling cm700.cpp to cm700.o"
	@$(CC) $(CFLAGS) -c ./src/cm700.cpp -o ./objects/cm700.o

serial.o: serial.cpp
	@mkdir -p objects
	@echo "Compiling serial.cpp to serial.o"
	@$(CC) $(CFLAGS) -c ./src/serial.cpp -o ./objects/serial.o

clean:
	@rm -f ./objects/*.o