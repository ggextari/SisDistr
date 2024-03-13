CC=gcc
CFLAGS=-Wall
LFLAGS=-lrpcsvc

all: inicializador_srvdns cliente estadis

inicializador_srvdns: inicializador_srvdns.c
    $(CC) $(CFLAGS) inicializador_srvdns.c -o inicializador_srvdns $(LFLAGS)

cliente: cliente.c
    $(CC) $(CFLAGS) cliente.c -o cliente $(LFLAGS)

estadis: estadis.c
    $(CC) $(CFLAGS) estadis.c -o estadis $(LFLAGS)

clean:
    rm -f inicializador_srvdns cliente estadis
