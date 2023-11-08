#include "Timekeeping.h"

ESP32Time rtc(0);  // create an instance with a specifed offset in seconds
bool dateConfigured = false;

