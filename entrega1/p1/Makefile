CC=gcc
CFLAGS=-Wall

all: srvdns cliente

srvdns: srvdns.c
    $(CC) $(CFLAGS) srvdns.c -o srvdns

cliente: cliente.c
    $(CC) $(CFLAGS) cliente.c -o cliente

clean:
    rm -f srvdns cliente