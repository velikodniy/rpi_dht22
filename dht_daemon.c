#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <sqlite3.h>

#include "dht.h"
#include "base.h"
#include "server.h"

#define PORT 8080
#define DBPATH "data.db"


int main(int argc, char **argv) {
  int port = -1;

  sqlite3 *db_server, *db_sensor;
  server_t server;

  int ret;
  int i;

  char* port_s = NULL;
  char* dbpath = NULL;

  // Parse options
  int c;  
  opterr = 0;
  while ((c = getopt (argc, argv, "hp:d:")) != -1)
    switch (c) {
    case 'h':
      printf ("USAGE: %s -p port -d dbpath\n", argv[0]);
      printf ("Defaults: port=%d, dbpath='%s'\n", PORT, DBPATH); 
      return 0;
    case 'p':
      port_s = optarg;
      port = atoi(port_s);
      break;
    case 'd':
      dbpath = optarg;
      break;
    case '?':
      if (optopt == 'p' || optopt == 'd')
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
  port = port == -1 ? PORT : port;
  dbpath = dbpath == NULL ? DBPATH : dbpath;
  
#ifdef DEBUG
  printf ("port = %d, dbpath = %s\n", port, dbpath);
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

  getchar();

  server_stop(server);
  base_close(db_server);

  return 0;
}
