CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

all: Server client

Server: Server.c
	$(CC) $(CFLAGS) -o Server Server.c $(LIB)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LIB)

clean:
	rm -f *.o client *~
	rm -f *.o Server *~

