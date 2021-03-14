#ifndef TEKDAQC_ZEPHYR_SNTP_H
#define TEKDAQC_ZEPHYR_SNTP_H

#include <logging/log.h>

#include <arpa/inet.h>
#include <net/sntp.h>
#include <posix/time.h>

#define SNTP_SERVER_ADDR "192.168.0.191" //"time.nist.gov"
#define SNTP_TIMEOUT 3000

void init_clock_via_sntp(void) {
  LOG_MODULE_DECLARE(network);
  LOG_INF("Syncing SNTP Time.");
  struct sntp_time ts;
  struct timespec tspec;
  int res = sntp_simple(SNTP_SERVER_ADDR, SNTP_TIMEOUT, &ts);

  if (res < 0) {
    LOG_ERR("Cannot fetch time using SNTP: %d", res);
    return;
  }

  tspec.tv_sec = ts.seconds;
  tspec.tv_nsec = ((uint64_t)ts.fraction * (1000 * 1000 * 1000)) >> 32;
  res = clock_settime(CLOCK_REALTIME, &tspec);
  if (res < 0) {
    LOG_ERR("Cannot set real clock time: %d", res);
    return;
  }
}

#endif // TEKDAQC_ZEPHYR_SNTP_H
