gcc -c dht.c -Wall -Wextra -Wconversion -I./bcm2835
gcc -c base.c -Wall -Wextra -Wconversion 
gcc -c server.c -Wall -Wno-unused-parameter -Wextra -Wconversion
gcc -c dht_daemon.c -Wall -Wno-unused-parameter -Wextra -Wconversion
gcc dht_daemon.c dht.o base.o server.o\
	-o dht_daemon\
	-lsqlite3 -lmicrohttpd -lbcm2835\
	-L./bcm2835 
