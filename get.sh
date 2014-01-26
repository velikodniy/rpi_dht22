wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.36.tar.gz -O bcm2835-1.36.tar.gz
tar xzf bcm2835-1.36.tar.gz
cd bcm2835-1.36
./configure
make
cd ..
mkdir bcm2835
cp bcm2835-1.36/src/bcm2835.h bcm2835/
cp bcm2835-1.36/src/libbcm2835.a bcm2835/