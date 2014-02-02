#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>

#include <sqlite3.h>

#include "dht.h"
#include "base.h"
#include "server.h"

#define PORT 8080
#define DBPATH "data.db"
#define TIMEINT (5*60)

// Server parameters
sqlite3 *db_server;
server_t server;

// Sensor reader parameters
sqlite3* db_sensor;
unsigned int timeint = 0;

void on_timer_sig(int arg) {
  double temp, hum;
#ifdef DEBUG
  printf("Got SIGALRM\n");
#endif
  dht_get(&temp, &hum);
  base_save(db_sensor, temp, hum);
  alarm(timeint);
}

void on_exit_sig(int arg) {
#ifdef DEBUG
  printf("Got SIGTERM\n");
#endif
  server_stop(server);
  base_close(db_server);
}

int main(int argc, char **argv) {
  int port = -1;

  int ret;
  int i;

  char* port_s = NULL;
  char* dbpath = NULL;
  char* timeint_s = NULL;

  // Parse options
  int c;  
  opterr = 0;
  while ((c = getopt (argc, argv, "hp:d:t:")) != -1)
    switch (c) {
    case 'h':
      printf ("USAGE: %s -p port -d dbpath -t interval\n", argv[0]);
      printf ("Defaults: port=%d, dbpath='%s', interval=%d\n", PORT, DBPATH, TIMEINT); 
      return 0;
    case 'p':
      port_s = optarg;
      port = atoi(port_s);
      break;
    case 'd':
      dbpath = optarg;
      break;
    case 't':
      timeint_s = optarg;
      ret = atoi(timeint_s);
      timeint = ret < 0 ? 0 : (unsigned int)ret;
      break;
    case '?':
      if (optopt == 'p' ||
	  optopt == 'd' ||
	  optopt == 't')
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,
		 "Unknown option character `\\x%x'.\n",
		 optopt);
      return 1;
    default:
      abort ();
    }

  // Set defaults when needed
  port = port < 0 ? PORT : port;
  dbpath = dbpath == NULL ? DBPATH : dbpath;
  timeint = timeint < 1 ? TIMEINT : timeint;
  
#ifdef DEBUG
  printf ("port = %d, dbpath = %s, timeint=%d\n", port, dbpath, timeint);
#endif

  if (optind < argc) {
    for (i = optind; i < argc; i++)
      printf ("Non-option argument %s\n", argv[i]);
    return 0;
  }

  // Start HTTP-server
  ret = base_init(&db_server, dbpath);
  if (ret != 0) {
    fprintf(stderr, "Cannot open base '%s'", dbpath);
    return 1;
  }
    
  server = server_start(port, db_server);
  if (server == NULL) {
    fprintf(stderr, "Cannot start server on port %d\n", port);
    return 1;
  }

  // Register signal handlers
  signal(SIGTERM, &on_exit_sig);
  signal(SIGALRM, &on_timer_sig);

  // Start sensor reader
  ret = dht_init();
  if (ret != 0) {
    fprintf(stderr, "Cannot init sensor");
    return 1;
  }
  ret = base_init(&db_sensor, dbpath);
  if (ret != 0) {
    fprintf(stderr, "Cannot open base '%s'", dbpath);
    return 1;
  }
  alarm(timeint);

  // Main loop
  while(1)
    pause();

  return 0;
}
