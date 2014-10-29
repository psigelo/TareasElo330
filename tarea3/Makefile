
VPATH = ./include ./src
COMPILER = gcc
COMPILER_FLAGS = -I./include
EXECUTABLE = erp_udp

erp_udp: erp_udp.c
	mkdir -p bin
	$(COMPILER) $(COMPILER_FLAGS) ./src/erp_udp.c -o ./bin/$(EXECUTABLE) -lpthread

clientePrueba: ./ArchivosPruebas/clienteUDP.c
	gcc ./ArchivosPruebas/clienteUDP.c -o ./bin/udpClientePrueba

prueba:
	./bin/$(EXECUTABLE) 50000 10000 50 56789 45678 

server: ./ArchivosPruebas/serverUDP.c
	gcc ./ArchivosPruebas/serverUDP.c -o ./bin/udpServer

pruebaClienteUDP:
	./bin/udpClientePrueba 127.0.0.1

clean:
	rm -R ./bin

