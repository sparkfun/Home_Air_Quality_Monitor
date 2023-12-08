#ifndef AQI_H
#define AQI_H

#include <Arduino.h>
#include <float16.h>

float get_composite_AQI(float ppm25, float ppm10, float);

#endif