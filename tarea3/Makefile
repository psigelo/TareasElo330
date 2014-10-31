
VPATH = ./include ./src
COMPILER = gcc
COMPILER_FLAGS = -I./include
EXECUTABLE = erp_udp

erp_udp: erp_udp.c
	mkdir -p bin
	$(COMPILER) $(COMPILER_FLAGS) ./src/erp_udp.c -o ./bin/$(EXECUTABLE) -lpthread

cliente: ./ArchivosPruebas/clienteUDP.c
	gcc ./ArchivosPruebas/clienteUDP.c -o ./bin/udpClient

server: ./ArchivosPruebas/serverUDP.c
	gcc ./ArchivosPruebas/serverUDP.c -o ./bin/udpServer

pruebaCliente:
	./bin/udpClient 127.0.0.1

pruebaServidor:
	./bin/udpServer

pruebaERP_UDP:
	./bin/erp_udp 5000000 4000000 15 56789 45678 

clean:
	rm -Rf ./bin

