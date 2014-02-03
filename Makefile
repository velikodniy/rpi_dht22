BCM2835VER=1.36
BCM2835DIR=./bcm2835

CC=gcc
LINKER=gcc
CFLAGS=-Wall -Wno-unused-parameter -Wextra -Wconversion

LIBS=-lsqlite3 -lmicrohttpd -lbcm2835
INCDIR=-I$(BCM2835DIR)
LIBDIR=-L$(BCM2835DIR)
OBJS=base.o server.o dht.o dht_daemon.o

all: dht_daemon
.PHONY : all

dht_daemon: $(OBJS)
	$(LINKER) -o $@ $^ $(CFLAGS) $(LIBDIR) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCDIR)

getlibs:
	mkdir -p "$(BCM2835DIR)"
	cd "$(BCM2835DIR)" &&\
	wget "http://www.airspayce.com/mikem/bcm2835/bcm2835-$(BCM2835VER).tar.gz" -O "bcm2835-$(BCM2835VER).tar.gz" &&\
	tar xzf "bcm2835-$(BCM2835VER).tar.gz" &&\
	cd "bcm2835-$(BCM2835VER)" &&\
	./configure && make
	cp "$(BCM2835DIR)/bcm2835-$(BCM2835VER)/src/bcm2835.h" "$(BCM2835DIR)"
	cp "$(BCM2835DIR)/bcm2835-$(BCM2835VER)/src/libbcm2835.a" "$(BCM2835DIR)"
	rm -r "$(BCM2835DIR)/bcm2835-$(BCM2835VER)"
	rm "$(BCM2835DIR)/bcm2835-$(BCM2835VER).tar.gz"
.PHONY : getlibs

clean:
	rm *.o
	rm dht_daemon
.PHONY : clean
