EXECUTABLE = Quadratot
VPATH = ./src ./headers 
CC = g++ -O3
CFLAGS = -g -Wall -I./src -I./headers -I../../NEAT/objects -I../../NEAT/headers -I../../HyperNeat/objects -I../../HyperNeat/headers -I../../RobotLIB/remoteApi -I../../RobotLIB/include -I../../RobotLIB/robotSimulator/objects -I../../RobotLIB/robotSimulator/headers -I../../RobotLIB/objects/headers -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
LDFLAGS = -lpthread
DATE = `date +'%F--%T'`
VAR = ./saveTrains/$(DATE)

all: Quadratot.o CalcFunctions.o Fitness.o SimFiles.o
	@mkdir -p bin/functions_files
	@mkdir -p bin/error_files
	@mkdir -p bin/simulation_files
	@mkdir -p bin/simulation_files/robot_position
	@mkdir -p bin/simulation_files/joints_position
	@mkdir -p bin/NEAT_organisms
	@rm -f ./bin/test
	@rm -f ./bin/functions_files/*
	@rm -f ./bin/error_files/*
	@rm -f ./bin/simulation_files/robot_position/*
	@rm -f ./bin/simulation_files/joints_position/*
	@rm -f ./bin/simulation_files/*.txt
	@rm -f ./bin/NEAT_organisms/*
	@cd ../../HyperNeat; make
	@cd ../../RobotLIB; make
	@echo "Linking all objects to ./bin/Quadratot"
	@$(CC) $(CFLAGS) ./objects/Fitness.o ./objects/SimFiles.o ./objects/CalcFunctions.o ./objects/Quadratot.o ../../RobotLIB/robotSimulator/objects/RobotSimulator.o ../../RobotLIB/objects/objects/Object.o ../../RobotLIB/objects/objects/CollisionObject.o ../../RobotLIB/objects/objects/Joint.o ../../RobotLIB/objects/objects/Dummy.o ../../NEAT/objects/NEAT.o ../../NEAT/objects/genetic_encoding.o ../../NEAT/objects/discrete_probabilities.o ../../NEAT/objects/random_function.o  ../../HyperNeat/objects/HyperNeat.o ../../HyperNeat/objects/Substrate.o ../../HyperNeat/objects/SpatialNode.o ../../HyperNeat/objects/CPPNInputs.o ../../HyperNeat/objects/UserFunctions.o ../../RobotLIB/remoteApi/extApiPlatform.o ../../RobotLIB/remoteApi/extApi.o -o ./bin/$(EXECUTABLE) $(LDFLAGS)

Quadratot.o: Quadratot.cpp 
	@mkdir -p objects
	@echo "Compiling Quadratot.cpp to Quadratot.o"
	@$(CC) $(CFLAGS) -c ./src/Quadratot.cpp -o ./objects/Quadratot.o

CalcFunctions.o: CalcFunctions.cpp 
	@mkdir -p objects
	@echo "Compiling CalcFunctions.cpp to CalcFunctions.o"
	@$(CC) $(CFLAGS) -c ./src/CalcFunctions.cpp -o ./objects/CalcFunctions.o

Fitness.o: Fitness.cpp
	@mkdir -p objects
	@echo "Compiling Fitness.cpp to Fitness.o"
	@$(CC) $(CFLAGS) -c ./src/Fitness.cpp -o ./objects/Fitness.o

SimFiles.o: SimFiles.cpp
	@mkdir -p objects
	@echo "Compiling SimFiles.cpp to SimFiles.o"
	@$(CC) $(CFLAGS) -c ./src/SimFiles.cpp -o ./objects/SimFiles.o

clean:
	@rm -f ./objects/*.o
	@rm -f ./bin/$(EXECUTABLE)
	@rm -f ./bin/test
	@rm -f ./bin/functions_files/*
	@rm -f ./bin/error_files/*
	@rm -f ./bin/NEAT_organisms/*
	@rm -f -R ./bin/simulation_files/*

save:
	@mkdir -p $(VAR); cp -R ./bin/* $(VAR);

