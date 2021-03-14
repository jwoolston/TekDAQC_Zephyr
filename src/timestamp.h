#ifndef TEKDAQC_ZEPHYR_LOGGING_H
#define TEKDAQC_ZEPHYR_LOGGING_H

#include <errno.h>
#include <logging/log.h>
#include <logging/log_ctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <posix/time.h>
#include <unistd.h>

static uint32_t get_timestamp() {
  LOG_MODULE_DECLARE(timestamp, LOG_LEVEL_DBG);
  struct timespec tspec;
  int res = clock_gettime(CLOCK_REALTIME, &tspec);

  if (res < 0) {
    LOG_ERR("Error retrieving time: %d", res);
    return 0;
  } else {
    return ((uint32_t)(tspec.tv_sec & 0xFFFFFFFF));
    //return tv.tv_usec;
  }
}

static uint32_t get_frequency() { return 1; }//e9; }

void configure_logging_timestamp() {
  /* Timestamp function could be set only from kernel thread. */
  (void)log_set_timestamp_func(get_timestamp, get_frequency());
}

#endif // TEKDAQC_ZEPHYR_LOGGING_H
