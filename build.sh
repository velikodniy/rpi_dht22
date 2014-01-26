gcc -c dht.c -Wall -Wextra -Wconversion -I./bcm2835 -L./bcm2835 -lbcm2835
gcc -c base.c -Wall -Wextra -Wconversion -lsqlite3
gcc -c server.c -Wall -Wno-unused-parameter -Wextra -Wconversion -lmicrohttpd -lsqlite3
