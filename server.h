#ifndef __SERVER_H__
#define __SERVER_H__

#include <microhttpd.h>

typedef struct MHD_Daemon* server_t;

int options200(struct MHD_Connection *connection);
int http400(struct MHD_Connection *connection);
int request_handler (void * dbv,
		     struct MHD_Connection *connection,
		     const char *url,
		     const char *method,
		     const char *version,
		     const char *upload_data, size_t *upload_data_size,
		     void **con_cls);
server_t server_start (int port, sqlite3* db);
void server_stop(server_t d);

#endif
