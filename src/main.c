#include <devicetree.h>
#include <stdio.h>
#include <zephyr.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include "network.h"
//#include "mdns_service.h"

//K_THREAD_DEFINE(dhcp_mdns, DHCP_MDNS_THREAD_STACKSIZE, initialize_dhcp, NULL,
//                NULL, NULL, DHCP_MDNS_THREAD_PRIORITY, 0, 0);

void main() {
  LOG_DBG("Starting main.");

  initialize_network();
  int i = 0;
  while (true) {
    i++;
    //LOG_DBG("Loop %d", i++);
    //k_sleep(K_MSEC(16));
  }
}
