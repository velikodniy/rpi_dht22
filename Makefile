LIBVER=1.36
LIBDIR=./bcm2835

CC=gcc
LINKER=gcc
CFLAGS=-Wall -Wno-unused-parameter -Wextra -Wconversion

LIBS=-lsqlite3 -lmicrohttpd -lbcm2835
OBJS=base.o server.o dht.o dht_daemon.o

all: dht_daemon
.PHONY: all

dht_daemon: $(OBJS)
	$(LINKER) -o $@ $^ $(CFLAGS) -L$(LIBDIR) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -I$(LIBDIR)

getlibs:
	mkdir -p "$(LIBDIR)"
	cd "$(LIBDIR)" &&\
	wget "http://www.airspayce.com/mikem/bcm2835/bcm2835-$(LIBVER).tar.gz" -O "bcm2835-$(LIBVER).tar.gz" &&\
	tar xzf "bcm2835-$(LIBVER).tar.gz" &&\
	cd "bcm2835-$(LIBVER)" &&\
	./configure && make
	cd "$(LIBDIR)" &&\
	cp "bcm2835-$(LIBVER)/src/bcm2835.h" ./ &&\
	cp "bcm2835-$(LIBVER)/src/libbcm2835.a" ./ &&\
	rm -r "bcm2835-$(LIBVER)" &&\
	rm "bcm2835-$(LIBVER).tar.gz"
.PHONY: getlibs

clean:
	rm *.o dht_daemon
.PHONY: clean
