#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "base.h"

#define BUFFER_SIZE 10*1024*1024

#define QCREATE "CREATE TABLE IF NOT EXISTS data("	\
  "id INTEGER PRIMARY KEY,"				\
  "temp REAL, hum REAL,"				\
  "time DATETIME DEFAULT CURRENT_TIMESTAMP"		\
  ");"
#define QINDEXTIME "CREATE INDEX IF NOT EXISTS idx_time ON data (time);"
#define QINDEXID   "CREATE INDEX IF NOT EXISTS idx_id ON data (id);"

#define QSAVE "INSERT INTO data (temp, hum) VALUES (%f, %f);"
#define QLOADN "SELECT id, time, temp, hum FROM data ORDER BY time DESC LIMIT %d;"
#define QLOADB "SELECT id, time, temp, hum FROM data WHERE time BETWEEN '%q' AND '%q';"

char* result_buffer;

int base_init (sqlite3** db, char* dbname) {
  result_buffer = malloc(BUFFER_SIZE * sizeof(char));
  if (result_buffer == NULL)
    return 1;

  if (sqlite3_open_v2 (dbname, db,
		       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		       NULL)) {
#ifdef DEBUG
    fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
#endif
    sqlite3_close_v2 (*db);
    return 1;
  }

  base_query (*db, QCREATE);
  base_query (*db, QINDEXTIME);
  base_query (*db, QINDEXID);

  return 0;
}

int base_save (sqlite3* db, double T, double H) {
  int ret;
  char* q = sqlite3_mprintf (QSAVE, T, H);
  ret = base_query (db, q);
  sqlite3_free (q);
  return ret;
}

int base_load_last (sqlite3* db, unsigned int N, char** result) {
  int ret;
  char* q = sqlite3_mprintf (QLOADN, N);
  ret = base_query_json (db, q, result);
  sqlite3_free (q);
  return ret;
}

int base_load_between (sqlite3* db, const char* from, const char* to, char** result) {
  int ret;
  char* q = sqlite3_mprintf (QLOADB, from, to);
  ret = base_query_json (db, q, result);
  sqlite3_free (q);
  return ret;
}

int base_query (sqlite3* db, const char* q) {
  int rc;
  char *zErrMsg = NULL;
  
  rc = sqlite3_exec (db, q, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
#ifdef DEBUG
    fprintf (stderr, "SQL error: %s\n", zErrMsg);
#endif
    sqlite3_free (zErrMsg);
    return 1;
  }
  return 0;
}

long buffer_est_size (char* p) {
  char* end = result_buffer + BUFFER_SIZE;
  return end - p;
}

int check_buffer (char* p, size_t n) {
  char* end = result_buffer + BUFFER_SIZE;
  return (p + n + 1) < end;
}

int add_to_buffer (char** pp, char* str) {
  size_t len = strlen(str);
  if (!check_buffer (*pp, len))
    return 1;
  strcpy (*pp, str);
  (*pp) += len;
  return 0;
}

// TODO Check it!
int result_iterate (void* json_v, int col_count, char** cols, char** col_names) {
  int i;
  long size, count;
  char** json = (char**)json_v;

  // Format string
  if (add_to_buffer (json, "{") != 0)
    return 1;
  
  for (i = 0; i < col_count; i++) {
    if (cols[i] == NULL)
      continue;
    
    size = buffer_est_size (*json) - 1;
    count = snprintf (*json, (size_t)size, "%s: '%s'", col_names[i], cols[i]);
    if (count >= size)
      return 1;
    (*json) += count;
    
    if (i != col_count - 1)
      if (add_to_buffer (json, ", ") != 0)
	return 1;
  }

  if (add_to_buffer (json, "}, ") != 0)
    return 1;

  return 0;
}

int base_query_json (sqlite3* db, const char* q, char** result) {
  int rc;
  char* zErrMsg = NULL;

  *result = result_buffer;

  strcpy (*result, "[");
  (*result)++;

  rc = sqlite3_exec (db, q, result_iterate, result, &zErrMsg);
  if (rc != SQLITE_OK) {
#ifdef DEBUG
    fprintf (stderr, "SQL error (%d): %s\n", rc, zErrMsg);
#endif
    sqlite3_free (zErrMsg);
    return 1;
  }

  (*result)[strlen(*result)-2] = ']';
 
  *result = result_buffer;

  return 0;
}

int base_close (sqlite3* db) {
  // Close SQLite connection
  free(result_buffer);
  return sqlite3_close_v2 (db);
}
