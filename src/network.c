#include "network.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(network, LOG_LEVEL_DBG);

#include "dhcp.h"

/* size of stack area used by each thread */
#define DHCP_MDNS_THREAD_STACKSIZE 1024

/* scheduling priority used by the thread */
#define DHCP_MDNS_THREAD_PRIORITY 7

void initialize_network() {
  initialize_dhcp();
  LOG_INF("Starting HTTP Server.");
  start_http_server();
  //sntp();
}