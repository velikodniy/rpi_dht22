#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

#include "dht.h"
#include "base.h"
#include "server.h"

int main(int argc, char **argv) {
  int ret;
  sqlite3* db;
  server_t server;

  const char* basename = "data.db";
  const int port = 8080;

  ret = base_init(&db, basename);
  if (ret != 0) {
    fprintf(stderr, "Cannot open base '%s'", basename);
    return 1;
  }
    
  server = server_start(port, db);
  if (server == NULL) {
    fprintf(stderr, "Cannot start server on port %d\n", port);
    return 1;
  }

  getchar();

  server_stop(server);
  base_close(db);

  return 0;
}
