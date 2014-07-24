#include <stdint.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "pi_dht_read.h"
#include "common_dht_read.h"

#include "dht.h"

#define PIN 4

void dht_get(float* temp, float* hum) {
  int errcode;
#ifdef DEBUG
  fprintf (stderr, "\n");
#endif
  // While checksum is not correct
  while((errcode = dht_read(PIN, temp, hum)) != DHT_SUCCESS)
#ifdef DEBUG
    switch(errcode) {
    case DHT_ERROR_TIMEOUT:
      fprintf (stderr, "DHT error: timeout\n");
      break;
    case DHT_ERROR_CHECKSUM:
      fprintf (stderr, "DHT error: incorrect checksum\n");
      break;
    case DHT_ERROR_ARGUMENT:
      fprintf (stderr, "DHT error: invalid argument\n");
      break;
    case DHT_ERROR_GPIO:
      fprintf (stderr, "DHT error: GPIO error\n");
      break;
    }
#endif
    ;
#ifdef DEBUG
    fprintf (stderr, "\n");
#endif
}

int dht_read(uint8_t pin, float* temp, float* hum) {
  return pi_dht_read(DHT22, pin, hum, temp);
}
