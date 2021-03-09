#ifndef TEKDAQC_ZEPHYR_RTC_H
#define TEKDAQC_ZEPHYR_RTC_H

#include <zephyr.h>
#include <device.h>
#include <drivers/counter.h>

#define RTC_0 DT_INST(0, st_stm32_rtc)
#if DT_NODE_HAS_STATUS(RTC_0, okay)
#define RTC_LABEL DT_LABEL(RTC_0)
#else
#error Your devicetree has no enabled nodes with compatible "st,stm32-rtc"
#define RTC_LABEL "<none>"
#endif

error_t rtc_configure();

error_t rtc_set_time(uint64_t seconds);

#endif // TEKDAQC_ZEPHYR_RTC_H
