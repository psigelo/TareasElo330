EXECUTABLE = HyperNeat
VPATH = ./src ./headers ./objects
CC = g++ -O3
CFLAGS = -g -Wall -I./headers -I./objects -I../NEAT/objects -I../NEAT/headers
LDFLAGS = -lpthread

all: CompilingNeat.o HyperNeat.o Substrate.o SpatialNode.o CPPNInputs.o UserFunctions.o  	
	
CompilingNeat.o:
	@cd ../NEAT; make NEAT.o

HyperNeat.o: HyperNeat.cpp
	@mkdir -p objects
	@echo "Compiling HyperNeat.cpp to HyperNeat.o"
	@$(CC) $(CFLAGS) -c ./src/HyperNeat.cpp  -o ./objects/HyperNeat.o

Substrate.o: Substrate.cpp
	@mkdir -p objects
	@echo "Compiling Substrate.cpp to Substrate.o"
	@$(CC) $(CFLAGS) -c ./src/Substrate.cpp  -o ./objects/Substrate.o

SpatialNode.o: SpatialNode.cpp
	@mkdir -p objects
	@echo "Compiling SpatialNode.cpp to SpatialNode.o"
	@$(CC) $(CFLAGS) -c ./src/SpatialNode.cpp  -o ./objects/SpatialNode.o

CPPNInputs.o: CPPNInputs.cpp 
	@mkdir -p objects
	@echo "Compiling CPPNInputs.cpp to CPPNInputs.o"
	@$(CC) $(CFLAGS) -c ./src/CPPNInputs.cpp  -o ./objects/CPPNInputs.o

UserFunctions.o: UserFunctions.cpp 
	@mkdir -p objects
	@echo "Compiling UserFunctions.cpp to UserFunctions.o"
	@$(CC) $(CFLAGS) -c ./src/UserFunctions.cpp -o ./objects/UserFunctions.o

CPPN-NEAT.o: CPPN-NEAT.cpp
	@mkdir -p objects
	@echo "Compiling CPPN-NEAT.cpp to CPPN-NEAT.o"
	@$(CC) $(CFLAGS) -c ./src/CPPN-NEAT.cpp -o ./objects/CPPN-NEAT.o

function.o: function.cpp
	@mkdir -p objects
	@echo "Compiling function.cpp to function.o"
	@$(CC) $(CFLAGS) -c ./src/function.cpp -o ./objects/function.o

genetic_encoding.o: genetic_encoding.cpp
	@mkdir -p objects
	@echo "Compiling genetic_encoding.cpp to genetic_encoding.o"
	@$(CC) $(CFLAGS) -c ./src/genetic_encoding.cpp -o ./objects/genetic_encoding.o

clean:
	@rm -f ./objects/*.o

