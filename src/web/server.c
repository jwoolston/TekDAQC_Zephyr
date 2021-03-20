#include "server.h"

#include <posix/pthread.h>
#include <zephyr.h>

#include "civetweb.h"
#include "http_handler.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(webserver, LOG_LEVEL_DBG);

#define HTTP_PORT 80
#define HTTPS_PORT 4443

#define CIVETWEB_MAIN_THREAD_STACK_SIZE CONFIG_MAIN_STACK_SIZE

/* Use smallest possible value of 1024 (see the line 18619 of civetweb.c) */
#define MAX_REQUEST_SIZE_BYTES 1024

K_THREAD_STACK_DEFINE(civetweb_stack, CIVETWEB_MAIN_THREAD_STACK_SIZE);

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

  init_http_server_handlers(ctx);

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
