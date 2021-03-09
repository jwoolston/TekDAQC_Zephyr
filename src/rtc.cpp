#include "rtc.h"
#include <drivers/counter.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(rtc, LOG_LEVEL_DBG);

static const struct device *rtc = nullptr;
static bool initialized = false;

error_t rtc_configure() {
  if (!initialized) {
    rtc = device_get_binding(RTC_LABEL);
    if (rtc == nullptr) {
      LOG_ERR("No STM32 RTC device available\n");
      return ENODEV;
    }
    counter_start(rtc);
    initialized = true;
  }
  return 0;
}

error_t rtc_set_time(uint64_t seconds) {
  if (!initialized) {
    return EIO;
  }
  return 0;
}