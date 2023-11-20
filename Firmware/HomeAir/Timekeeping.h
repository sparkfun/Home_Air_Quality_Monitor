#ifndef TIMEKEEPING_H
#define TIMEKEEPING_H

#include "ESP32Time.h"

// Time Constants
const uint32_t ONE_DAY_MS = 86400000;
const uint32_t ONE_DAY_SEC = 86400;
const uint16_t ONE_HOUR_SEC = 3600;
const uint32_t ONE_HOUR_MS = ONE_HOUR_SEC * 1000;

extern ESP32Time rtc;
extern bool dateConfigured;

#endif