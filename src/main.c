#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <stdio.h>
#include "network.h"

void main() {
    printf("Starting main.\n");

    initialize_dhcp();
    int i = 0;
    while (true) {
    	printf("Loop %d", i++);
        k_sleep(K_MSEC(16));
    }
}
