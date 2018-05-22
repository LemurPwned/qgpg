CFLAGS = -Wall -lsodium
GDB = -g

GLOB_OBJ = server.o client.o sock_utils.o proto.o
SERV_DEP = server.o sock_utils.o
DEPS = proto.o sock_utils.o

CVER = -std=gnu11
all: app

app: $(GLOB_OBJ)
	gcc $(CFLAGS) client.c -o client
	gcc $(CFLAGS) $(SERV_DEP) -o server

debug: $(GLOB_OBJ)
	gcc $(GDB) $(CFLAGS) client.c -o client
	gcc $(GDB) $(CFLAGS) $(SERV_DEP) -o server

server.o: proto.o server.c
	gcc -c server.c

client.o: proto.o client.c

proto.o: qgpg_proto_def.c sock_utils.o
	gcc $(CFLAGS) -c qgpg_proto_def.c -lsodium

clean:
	rm -rf *.o
	rm -f server client
