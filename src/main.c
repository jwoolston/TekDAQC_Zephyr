#include "network.h"
#include <device.h>
#include <devicetree.h>
#include <stdio.h>
#include <zephyr.h>

K_THREAD_DEFINE(dhcp_mdns, DHCP_MDNS_THREAD_STACKSIZE, initialize_dhcp, NULL,
                NULL, NULL, DHCP_MDNS_THREAD_PRIORITY, 0, 0);

void main() {
  printf("Starting main.\n");

  initialize_network();
  int i = 0;
  while (true) {
    printf("Loop %d", i++);
    k_sleep(K_MSEC(16));
  }
}
