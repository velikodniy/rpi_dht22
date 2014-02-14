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
int port = -1;

// Sensor reader parameters
sqlite3* db_sensor;
unsigned int timeint = 0;

// DB parameters
char* dbpath = NULL;

void on_timer_sig (int arg) {
  double temp, hum;
#ifdef DEBUG
  fprintf (stderr, "Got SIGALRM\n");
#endif
  dht_get (&temp, &hum);
  base_save (db_sensor, temp, hum);
  alarm (timeint);
}

void on_exit_sig(int arg) {
#ifdef DEBUG
  fprintf (stderr, "Exit\n");
#endif
  server_stop (server);
  base_close (db_server);
  exit (1);
}

void print_usage(char *name) {
  printf ("USAGE:\n\t%s [-p port] [-d dbpath] [-t interval] [-s]\n", name);
  printf ("Options:\n");
  printf ("\t-s\tRun only HTTP server\n");
  printf ("Defaults:\n");
  printf ("\tport=%d\n", PORT);
  printf ("\tdbpath='%s'\n", DBPATH);
  printf ("\tinterval=%d\n", TIMEINT);

}

void http_start (void) {
  int ret;
#ifdef DEBUG
  fprintf (stderr, "Starting HTTP-server...");
#endif
  ret = base_init (&db_server, dbpath);
  if (ret != 0) {
    fprintf (stderr, "Cannot open base '%s'\n", dbpath);
    exit (1);
  }
  
  server = server_start(port, db_server);
  if (server == NULL) {
    fprintf (stderr, "Cannot start server on port %d\n", port);
    exit (1);
  }
#ifdef DEBUG
  fprintf (stderr, "OK\n");
#endif
}

void sensor_start(void) {
  int ret;
#ifdef DEBUG
  fprintf (stderr, "Starting sensor reader...");
#endif
  ret = dht_init();
  if (ret != 0) {
    fprintf (stderr, "Cannot init sensor\n");
    exit (1);
  }
  ret = base_init(&db_sensor, dbpath);
  if (ret != 0) {
    fprintf (stderr, "Cannot open base '%s'\n", dbpath);
    exit (1);
  }
  signal (SIGALRM, &on_timer_sig);
  alarm (timeint);
#ifdef DEBUG
  fprintf (stderr, "OK\n");
#endif
}

int main (int argc, char **argv) {
  int ret;
  int i;

  char* port_s = NULL;
  char* timeint_s = NULL;

  int http_only = 0;

  // Parse options
  int c;  
  opterr = 0;
  while ((c = getopt (argc, argv, "hp:d:t:s")) != -1)
    switch (c) {
    case 'h':
      print_usage (argv[0]);
      return 0;
    case 'p':
      port_s = optarg;
      port = atoi (port_s);
      break;
    case 'd':
      dbpath = optarg;
      break;
    case 't':
      timeint_s = optarg;
      ret = atoi (timeint_s);
      timeint = ret < 0 ? 0 : (unsigned int)ret;
      break;
    case 's':
      http_only = 1;
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
      exit (1);
    default:
      abort ();
    }

  // Set defaults when needed
  port = port < 0 ? PORT : port;
  dbpath = dbpath == NULL ? DBPATH : dbpath;
  timeint = timeint < 1 ? TIMEINT : timeint;
  
#ifdef DEBUG
  fprintf (stderr, "port = %d,\ndbpath = %s,\ntimeint = %d\n", port, dbpath, timeint);
#endif

  if (optind < argc) {
    for (i = optind; i < argc; i++)
      printf ("Non-option argument %s\n", argv[i]);
    exit (1);
  }

  // Start HTTP-server
  http_start ();

  // Start sensor reader
  if (!http_only) {
    sensor_start ();
  }

  // Register exit signal handlers
  signal (SIGTERM, &on_exit_sig);
  signal (SIGINT,  &on_exit_sig);

  // Main loop
  while (1)
    pause ();

  return 0;
}
