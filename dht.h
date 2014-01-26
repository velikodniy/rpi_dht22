#ifndef __DHT_H__
#define __DHT_H__

#include <stdint.h>

int dht_init();
void dht_get(double* temp, double* hum);
int dht_read(uint8_t pin, double* temp, double* hum);

#endif
