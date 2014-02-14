NAME=dht_daemon

SOURCES=base.c server.c dht.c dht_daemon.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

LIBVER=1.36
LIBDIR=./bcm2835

CFLAGS += -Wall -Wno-unused-parameter -Wextra -Wconversion
CFLAGS += -I$(LIBDIR)
LDFLAGS += -L$(LIBDIR)
LDLIBS += -lsqlite3 -lmicrohttpd -lbcm2835

.PHONY: all getlibs clean

all: $(NAME)

$(NAME): $(OBJECTS)

clean:
	$(RM) *.o dht_daemon

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
