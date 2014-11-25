EXECUTABLE = example
CC = g++ -O3
CFLAGS = -g -Wall -I../remoteApi -I../include -I../robotSimulator/objects -I../robotSimulator/headers -I../objects/headers -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
LDFLAGS = -lpthread

all: example.o 
	@mkdir -p bin
	@mkdir -p error_files
	@rm -f /error_files/*
	@cd ..; make
	@echo "Linking all objects to ./bin/example"
	@$(CC) $(CFLAGS) example.o ../robotSimulator/objects/RobotSimulator.o ../objects/objects/Object.o ../objects/objects/Joint.o ../objects/objects/Dummy.o ../remoteApi/extApiPlatform.o ../remoteApi/extApi.o -o ./bin/$(EXECUTABLE) $(LDFLAGS)

example.o: example.cpp 
	@echo "Compiling example.cpp to example.o"
	@$(CC) $(CFLAGS) -c example.cpp -o example.o

run: 
	./bin/$(EXECUTABLE)

clean:
	@rm -f *.o
	@rm -f ./bin/$(EXECUTABLE)
	@rm -f ./error_files/*
