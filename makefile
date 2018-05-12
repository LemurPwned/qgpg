CFLAGS = -Wall
GDB = -g

GLOB_OBJ = server.o client.o sock_utils.o
SERV_DEP = server.o sock_utils.o

CVER = -std=gnu11
all: app

app: $(GLOB_OBJ)
	gcc $(CFLAGS) client.c -o client
	gcc $(CFLAGS) $(SERV_DEP) -o server

debug: $(GLOB_OBJ)
	gcc $(GDB) $(CFLAGS) client.c -o client
	gcc $(GDB) $(CFLAGS) $(SERV_DEP) -o server

server.o: server.c
	gcc -c server.c

client.o: client.c

clean:
	rm -rf *.o
	rm -f server client
