#include "network.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(network, LOG_LEVEL_DBG);

#include "dhcp.h"
#include "webserver.h"

void initialize_network() {
  initialize_dhcp();
  LOG_INF("Starting HTTP Server.");
  start_http_server();
}