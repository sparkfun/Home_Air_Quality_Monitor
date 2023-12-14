#ifndef AQI_H
#define AQI_H

#include <Arduino.h>
#include <float16.h>

float aqiGetCompositeAQI(float ppm25, float ppm10, float);

#endif