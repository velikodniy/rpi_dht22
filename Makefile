NAME=dht_daemon

SOURCES=base.c server.c dht.c dht_daemon.c pi_dht_read.c pi_mmio.c common_dht_read.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

LIBDIR=.

CFLAGS += -Wall -Wno-unused-parameter -Wextra -Wconversion
CFLAGS += -I$(LIBDIR)
LDFLAGS += -L$(LIBDIR)
LDLIBS += -lsqlite3 -lmicrohttpd

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJECTS)

clean:
	$(RM) *.o dht_daemon
