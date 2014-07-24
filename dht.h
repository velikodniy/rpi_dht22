#ifndef __DHT_H__
#define __DHT_H__

#include <stdint.h>

int dht_init();
void dht_get(float* temp, float* hum);
int dht_read(uint8_t pin, float* temp, float* hum);

#endif
