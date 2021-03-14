#include "webserver.h"

#include <data/json.h>
#include <posix/pthread.h>
#include <zephyr.h>

#include "civetweb.h"
#include "sdcard.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(webserver, LOG_LEVEL_DBG);

#define HTTP_PORT 80
#define HTTPS_PORT 4443

#define WEBSERVER_ROOT "web"
#define STREAM_BUFFER_SIZE 4096

#define CIVETWEB_MAIN_THREAD_STACK_SIZE CONFIG_MAIN_STACK_SIZE

/* Use smallest possible value of 1024 (see the line 18619 of civetweb.c) */
#define MAX_REQUEST_SIZE_BYTES 1024

K_THREAD_STACK_DEFINE(civetweb_stack, CIVETWEB_MAIN_THREAD_STACK_SIZE);

struct civetweb_info {
  const char *version;
  const char *os;
  uint32_t features;
  const char *feature_list;
  const char *build;
  const char *compiler;
  const char *data_model;
};

#define FIELD(struct_, member_, type_)                                         \
  {                                                                            \
    .field_name = #member_, .field_name_len = sizeof(#member_) - 1,            \
    .offset = offsetof(struct_, member_), .type = type_                        \
  }


static int stream_file(struct mg_connection *conn, const char *path) {
  static char uri_path[4096];
  static struct fs_dirent entry;
  static struct fs_file_t zfp;
  strcpy(uri_path, get_disk_mount());
  strcat(uri_path, "/");
  strcat(uri_path, WEBSERVER_ROOT);
  strcat(uri_path, path);

  int err = fs_stat(uri_path, &entry);
  if (err) {
    LOG_ERR("Unable to retrieve file information: %s. Reason: %d", uri_path, err);
    return 404;
  }
  err = fs_open(&zfp, uri_path, FS_O_READ);
  if (err) {
    LOG_ERR("Failed to open file for read: %s. Reason: %d", uri_path, err);
    return 500;
  }
  static uint8_t buffer[STREAM_BUFFER_SIZE];
  int remaining = entry.size;
  while (remaining > 0) {
    ssize_t count = remaining > STREAM_BUFFER_SIZE ? STREAM_BUFFER_SIZE : remaining;
    err = fs_read(&zfp, buffer, count);
    if (err) {
      LOG_ERR("Failed to read data from file: %s. Reason. %d", uri_path, err);
    }
    err = mg_write(conn, buffer, count);
    if (err > 0) {
      remaining -= err;
    } else if (err == 0) {
      LOG_WRN("Unable to write %d bytes to connection which has already been closed.", count);
      return 500;
    } else {
      LOG_ERR("Error writing %d bytes to connection.", count);
      return 500;
    }
  }
  err = fs_close(&zfp);
  if (err) {
    LOG_ERR("Failed to close file: %s. Reason: %d", uri_path, err);
  }
  return 200;
}

void send_ok(struct mg_connection *conn) {
  mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html\r\n"
                  "Connection: close\r\n\r\n");
}

int system_info_handler(struct mg_connection *conn, void *cbdata) {
  static const struct json_obj_descr descr[] = {
      FIELD(struct civetweb_info, version, JSON_TOK_STRING),
      FIELD(struct civetweb_info, os, JSON_TOK_STRING),
      FIELD(struct civetweb_info, feature_list, JSON_TOK_STRING),
      FIELD(struct civetweb_info, build, JSON_TOK_STRING),
      FIELD(struct civetweb_info, compiler, JSON_TOK_STRING),
      FIELD(struct civetweb_info, data_model, JSON_TOK_STRING),
  };

  struct civetweb_info info = {};
  char info_str[1024] = {};
  int ret;
  int size;

  size = mg_get_system_info(info_str, sizeof(info_str));

  ret = json_obj_parse(info_str, size, descr, ARRAY_SIZE(descr), &info);

  send_ok(conn);

  if (ret < 0) {
    mg_printf(conn, "Could not retrieve: %d\n", ret);
    return 500;
  }

  mg_printf(conn, "<html><body>");

  mg_printf(conn, "<h3>Server info</h3>");
  mg_printf(conn, "<ul>\n");
  mg_printf(conn, "<li>host os - %s</li>\n", info.os);
  mg_printf(conn, "<li>server - civetweb %s</li>\n", info.version);
  mg_printf(conn, "<li>compiler - %s</li>\n", info.compiler);
  mg_printf(conn, "<li>board - %s</li>\n", CONFIG_BOARD);
  mg_printf(conn, "</ul>\n");

  mg_printf(conn, "</body></html>\n");

  return 200;
}

int file_handler(struct mg_connection *conn, void *cbdata) {
  const struct mg_request_info* request = mg_get_request_info(conn);
  LOG_DBG("Processing request for file: %s", request->request_uri);
  return stream_file(conn, request->request_uri);
}

void *server_pthread(void *arg) {
  LOG_DBG("Server thread initializing.");
  static const char *const options[] = {"listening_ports",
                                        STRINGIFY(HTTP_PORT),
                                        "num_threads",
                                        "1",
                                        "max_request_size",
                                        STRINGIFY(MAX_REQUEST_SIZE_BYTES),
                                        NULL};

  struct mg_callbacks callbacks;
  struct mg_context *ctx;

  (void)arg;

  memset(&callbacks, 0, sizeof(callbacks));
  ctx = mg_start(&callbacks, 0, (const char **)options);

  if (ctx == NULL) {
    LOG_DBG("Unable to start the server.");
    return 0;
  }

  //mg_set_request_handler(ctx, "/$", hello_world_handler, 0);
  //mg_set_request_handler(ctx, "/info$", system_info_handler, 0);
  mg_set_request_handler(ctx, "/**", file_handler, 0);

  LOG_DBG("Webserver started.");
  return 0;
}

void start_http_server(void) {
  pthread_attr_t civetweb_attr;
  pthread_t civetweb_thread;

  (void)pthread_attr_init(&civetweb_attr);
  (void)pthread_attr_setstack(&civetweb_attr, &civetweb_stack,
                              CIVETWEB_MAIN_THREAD_STACK_SIZE);

  LOG_DBG("Creating HTTP server thread.");
  (void)pthread_create(&civetweb_thread, &civetweb_attr, &server_pthread, 0);
  LOG_DBG("HTTP server thread created.");
}
