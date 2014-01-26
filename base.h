#ifndef __BASE_H__
#define __BASE_H__

#include <sqlite3.h>

int base_init(sqlite3** db, char* dbname);
int base_save(sqlite3* db, double T, double H);
int base_load_last(sqlite3* db, unsigned int N, char** result);
int base_load_between(sqlite3* db, const char* from, const char* to, char** result);
int base_query (sqlite3* db, const char* q);
int result_iterate (void* json_res, int col_count, char** cols, char** col_names);
int base_query_json (sqlite3* db, const char* q, char** result);
int base_close(sqlite3* db);
#endif
