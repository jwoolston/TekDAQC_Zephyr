#ifndef TEKDAQC_ZEPHYR_HTTP_HANDLER_H
#define TEKDAQC_ZEPHYR_HTTP_HANDLER_H

#include <civetweb.h>

void init_http_server_handlers(struct mg_context *ctx);

#endif // TEKDAQC_ZEPHYR_HTTP_HANDLER_H
