#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <bcm2835.h>

#include "dht.h"

#define PIN 4

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define MAXTIMINGS 100

int dht_init() {
  // Initialize GPIO
  if (!bcm2835_init())
    return 1;
  return 0;
}

void dht_get(double* temp, double* hum) {
#ifdef DEBUG
  printf("\n");
#endif
  // While checksum is not correct
  while(dht_read(PIN, temp, hum) != 0)
#ifdef DEBUG
    printf(".");
#endif
    ;
#ifdef DEBUG
    printf("\n");
#endif
}

int dht_read(uint8_t pin, double* temp, double* hum) {
  int data[100];
  int counter = 0;
  int laststate = HIGH;
  int j=0;
  int i=0;
  
  // Set GPIO pin to output
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(pin, HIGH);
  usleep(100);
  bcm2835_gpio_write(pin, LOW);
  usleep(20000);
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
  
  // Read data
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  
  for (i = 0; i < MAXTIMINGS; i++) {
    counter = 0;
    while (bcm2835_gpio_lev(pin) == laststate) {
      struct timespec t = {0, 1L};
      nanosleep(&t, NULL);           // overclocking might change this?
      counter++;
      if (counter == 100)
	break;
    }
    laststate = bcm2835_gpio_lev(pin);
    if (counter == 100)
      break;
    
    if ((i>3) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > 16)
	data[j/8] |= 1;
      j++;
    }
  }
  
#ifdef DEBUG
  printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);
#endif
 
  // Validate checksum
  if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
    // Convert values
    *hum = (data[0] * 256 + data[1]) / 10.0;
    *temp = ((data[2] & 0x7F)* 256 + data[3]) / 10.0;
    if (data[2] & 0x80)
      *temp *= -1;
  } else
    return 1;

#ifdef DEBUG
  printf("T=%lf, H=%lf", *temp, *hum);
#endif
  
  return 0;
}
