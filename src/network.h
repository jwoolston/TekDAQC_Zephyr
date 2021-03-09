#ifndef TEKDAQC_ZEPHYR_NETWORK_H
#define TEKDAQC_ZEPHYR_NETWORK_H

#include <logging/log.h>
LOG_MODULE_REGISTER(network, LOG_LEVEL_DBG);

#include "dhcp.h"

/* size of stack area used by each thread */
#define DHCP_MDNS_THREAD_STACKSIZE 1024

/* scheduling priority used by the thread */
#define DHCP_MDNS_THREAD_PRIORITY 7

void initialize_network() {
  initialize_dhcp();
}

#endif // TEKDAQC_ZEPHYR_NETWORK_H
