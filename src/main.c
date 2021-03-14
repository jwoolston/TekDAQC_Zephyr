#include <stdio.h>
#include "sdcard.h"
#include "network.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

void main() {
  LOG_DBG("Starting main.");

  /*LOG_DBG("Configuring logging timestamp.");
  configure_logging_timestamp();*/

  LOG_DBG("Initializing sdcard.");
  initialize_disk_access();

  LOG_DBG("Initializing network.");
  initialize_network();
}