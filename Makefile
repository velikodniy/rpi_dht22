NAME=dht_daemon
BCM2835VER=1.36

getlibs:
	mkdir bcm2835
	cd bcm2835 &&\
	wget http://www.airspayce.com/mikem/bcm2835/bcm2835-$(BCM2835VER).tar.gz -O bcm2835-$(BCM2835VER).tar.gz &&\
	tar xzf bcm2835-$(BCM2835VER).tar.gz &&\
	cd bcm2835-$(BCM2835VER) &&\
	./configure && make &&\
	cp src/bcm2835.h ../ &&\
	cp src/libbcm2835.a ../
	rm -r bcm2835/bcm2835-$(BCM2835VER)
	rm bcm2835/bcm2835-$(BCM2835VER).tar.gz

clean:
	rm *.o
	rm $(NAME)
