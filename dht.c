#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <bcm2835.h>
#include <sqlite3.h>

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define MAXTIMINGS 100

int query(sqlite3* db, const char* q);
int readDHT(uint8_t pin, double* temp, double* hum);
void usleep(int usec);

int main(int argc, char **argv) {
  // Initialize GPIO
  if (!bcm2835_init())
    return 1;

  // Initialize SQLite
  sqlite3 *db;
  if (sqlite3_open("data.db", &db)){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }    
  
  uint8_t dhtpin = 4;
  
  // While checksum is not correct
  double T, H;
  while(readDHT(dhtpin, &T, &H) != 0);

  printf("Temp =  %.1f °C, Hum = %.1f %%\n", T, H);

  const char* Qct =
    "CREATE TABLE IF NOT EXISTS data("
    "id INTEGER PRIMARY KEY,"
    "temp REAL, hum REAL,"
    "time DATETIME DEFAULT CURRENT_TIMESTAMP"
    ");";
  const char* Qin_ =
    "INSERT INTO data (temp, hum) VALUES (%f, %f);";

  const size_t BUFSIZE = 128;
  char Qin[BUFSIZE];

  query(db, Qct);
  
  snprintf(Qin, BUFSIZE, Qin_, T, H);
  query(db, Qin);
  
  // Close SQLite
  sqlite3_close(db);
  return 0;
}

int query(sqlite3* db, const char* q) {
  int rc;
  char *zErrMsg = NULL;

  rc = sqlite3_exec(db, q, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return -1;
  }
  return 0;
}

int readDHT(uint8_t pin, double* temp, double* hum) {
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
    return -1;
  return 0;
}
