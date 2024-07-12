all: SHM_server SHM_client

SHM_server: datawarehousestructs.o SHM_server.o
	g++ datawarehousestructs.hpp SHM_server.o -o bin/shm_server

SHM_client: datawarehousestructs.o SHM_client.o
	g++ datawarehousestructs.hpp SHM_client.o -o bin/shm_client

datawarehousestructs.o: datawarehousestructs.o
	g++ datawarehousestructs.hpp

clean:
	rm -rf *.o
