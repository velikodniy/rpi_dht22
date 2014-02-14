#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "base.h"

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

int base_init(sqlite3** db, char* dbname) {
  if (sqlite3_open(dbname, db)){
#ifdef DEBUG
    fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
#endif
    sqlite3_close(*db);
    return 1;
  }

  base_query(*db, QCREATE);
  base_query(*db, QINDEXTIME);
  base_query(*db, QINDEXID);

  return 0;
}

int base_save(sqlite3* db, double T, double H) {
  int ret;
  char* q = sqlite3_mprintf(QSAVE, T, H);
  ret = base_query(db, q);
  sqlite3_free(q);
  return ret;
}

int base_load_last(sqlite3* db, unsigned int N, char** result) {
  int ret;
  char* q = sqlite3_mprintf(QLOADN, N);
  ret = base_query_json(db, q, result);
  sqlite3_free(q);
  return ret;
}

int base_load_between(sqlite3* db, const char* from, const char* to, char** result) {
  int ret;
  char* q = sqlite3_mprintf(QLOADB, from, to);
  ret = base_query_json(db, q, result);
  sqlite3_free(q);
  return ret;
}

int base_query (sqlite3* db, const char* q) {
  int rc;
  char *zErrMsg = NULL;
  
  rc = sqlite3_exec(db, q, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
#ifdef DEBUG
    fprintf (stderr, "SQL error: %s\n", zErrMsg);
#endif
    sqlite3_free(zErrMsg);
    return 1;
  }
  return 0;
}

// FIXME: Use large buffer instead copying
int result_iterate (void* json_res_v, int col_count, char** cols, char** col_names) {
  char* json;
  char* json_it;
  char* json_bak;
  char** json_res = (char**)json_res_v;
  int count, i;

  // Count chars
  unsigned long length = 0;
  for (i = 0; i < col_count; i++) {
    if (cols[i] == NULL)
      continue;
    length +=
      strlen(col_names[i]) +
      strlen(": '") +
      strlen(cols[i]) +
      strlen("'");

    if (i != col_count-1)
      length += strlen(", ");
  }

  length += strlen("{}, ");
  length += 1; // For '\0'

  // Malloc
  json = malloc(sizeof(char) * length);
  json_it = json;

  // Format string
  strcpy(json_it, "{");
  json_it++;
  for (i = 0; i < col_count; i++) {
    if (cols[i] == NULL)
      continue;
    count = sprintf(json_it, "%s: '%s'", col_names[i], cols[i]);
    json_it += count;
    
    if (i != col_count - 1) {
      strcat(json_it, ", ");
      json_it += strlen(", ");
    }
  }
  strcat(json_it, "}, ");

  // Add to result
  json_bak = *json_res;
  *json_res = malloc(sizeof(char) * (json_bak == NULL ? 0 : strlen(json_bak) +
				     strlen(json) +
				     1));
  strcpy(*json_res, json_bak);
  strcat(*json_res, json);
  free(json_bak);
  free(json);

  return 0;
}

int base_query_json (sqlite3* db, const char* q, char** result) {
  int rc;
  char *zErrMsg = NULL;
  *result = malloc(2 * sizeof(char));
  strcpy(*result, "[");

  rc = sqlite3_exec(db, q, result_iterate, result, &zErrMsg);
  if (rc != SQLITE_OK) {
#ifdef DEBUG
    fprintf (stderr, "SQL error (%d): %s\n", rc, zErrMsg);
#endif
    sqlite3_free(zErrMsg);
    free(*result);
    return 1;
  }

  if (strlen(*result) < 2) {
    char* res = malloc(sizeof(char) * 3);
    strcpy(res, "[]");
    free(*result);
    *result = res;
    return 0;
  }

  (*result)[strlen(*result)-2] = ']';
  return 0;
}

int base_close(sqlite3* db) {
  // Close SQLite
  sqlite3_close(db);
  return 0;
}
