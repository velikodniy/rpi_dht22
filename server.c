#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <microhttpd.h>
#include <sqlite3.h>

#include "base.h"
#include "server.h"

int http400(struct MHD_Connection *connection) {
  const char* msg = "400 Bad request";
  struct MHD_Response* response = MHD_create_response_from_buffer (strlen (msg),
								   (void *) msg,
								   MHD_RESPMEM_MUST_COPY);
  int ret = MHD_queue_response (connection, MHD_HTTP_BAD_REQUEST, response);
  MHD_destroy_response (response);
  return ret;
}

int request_handler (void * dbv,
		     struct MHD_Connection *connection,
		     const char *url,
		     const char *method,
		     const char *version,
		     const char *upload_data, size_t *upload_data_size,
		     void **con_cls) {
  static int con_cls0;
  struct MHD_Response* response;
  int ret;
  char* result;
  sqlite3* db = (sqlite3*) dbv;
  
  // Unexpected method
  if (0 != strcmp(method, "GET"))
    return http400(connection);
  
  // Do never respond on first call
  if (&con_cls0 != *con_cls) {
    *con_cls = &con_cls0;
    return MHD_YES;
  }
  *con_cls = NULL; // reset when done
  
  if (strcmp(url, "/last") == 0) {
    const char* count_s = MHD_lookup_connection_value(connection,
						      MHD_GET_ARGUMENT_KIND,
						      "count");
    if (count_s == NULL)
      return http400(connection);
    int count = atoi(count_s);
    if (count <= 0)
      return http400(connection);
    ret = base_load_last(db, (unsigned int)count, &result);
    if (ret != 0)
      return http400(connection);
  } else if (strcmp(url, "/between") == 0) {
    const char* from_s = MHD_lookup_connection_value(connection,
					       MHD_GET_ARGUMENT_KIND,
					       "from");
    const char* to_s = MHD_lookup_connection_value(connection,
					     MHD_GET_ARGUMENT_KIND,
					     "to");
    if ((from_s == NULL) || (to_s == NULL))
      return http400(connection);
    ret = base_load_between(db, from_s, to_s, &result);
    if (ret != 0)
      return http400(connection);
  } else
    return http400(connection);

  response = MHD_create_response_from_buffer (strlen (result),
					      (void *) result,
					      MHD_RESPMEM_MUST_COPY);
  free(result);
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);
  return ret;
}

server_t server_start (int port, sqlite3* db) {
  struct MHD_Daemon *d;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
  d = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY,
                        port,
                        NULL, NULL,
			&request_handler, db,
			MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 120,
			MHD_OPTION_END);
#pragma GCC diagnostic pop
  return d;
}

void server_stop(server_t d) {
  MHD_stop_daemon (d);
}
