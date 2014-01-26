// gcc test_base.c base.c -lsqlite3 -g -DDEBUG

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#include "base.h"

int main () {
  
  int ret;
  sqlite3* db;
  ret = base_init(&db, "data.db");
  char* result;

  ret = base_save(db, random() % 100 - 50, random() % 100 - 50);

  ret = base_load_last(db, 2, &result);
  if (ret == 0) {
    printf("%s\n", result);
    free(result);
  } else
    printf("Error!\n");
    
  ret = base_load_between(db, "2014-01-26 19:59:00", "2014-01-26 20:00:00", &result);
  if (ret == 0) {
    printf("%s\n", result);
    free(result);
  }
  else
    printf("Error!\n");
  
  base_close(db);
}
