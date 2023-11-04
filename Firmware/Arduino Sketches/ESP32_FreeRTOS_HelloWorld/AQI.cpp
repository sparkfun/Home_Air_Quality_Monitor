#include "AQI.h"

uint16_t get_ppm_25_AQI(float16 ppm25) {
  /*
    Returns the AQI corresponding to given ppm25 concentration
  */

  float16 indexHigh, indexLow;
  float16 breakpointHigh, breakpointLow;

  if (ppm25 > 0 && ppm25 < 12.0) {
    indexHigh = 50;
    indexLow = 0;
    breakpointHigh = 12.0;
    breakpointLow = 0;
  } else if (ppm25 > 12.0 && ppm25 < 35.4) {
    indexHigh = 100;
    indexLow = 51;
    breakpointHigh = 35.4;
    breakpointLow = 12.0;
  } else if (ppm25 > 35.4 && ppm25 < 55.4) {
    indexHigh = 150;
    indexLow = 101;
    breakpointHigh = 55.4;
    breakpointLow = 35.5;
  } else if (ppm25 > 55.4 && ppm25 < 150.4) {
    indexHigh = 151;
    indexLow = 200;
    breakpointHigh = 150.4;
    breakpointLow = 55.5;
  } else if (ppm25 > 150.4 && ppm25 < 250.4) {
    indexHigh = 201;
    indexLow = 300;
    breakpointHigh = 250.4;
    breakpointLow = 150.5;
  } else if (ppm25 > 250.4 && ppm25 < 350.4) {
    indexHigh = 400;
    indexLow = 301;
    breakpointHigh = 350.4;
    breakpointLow = 250.5;
  } else if (ppm25 > 350.4 && ppm25 < 500) {
    indexHigh = 500;
    indexLow = 401;
    breakpointHigh = 500;
    breakpointLow = 350.5;
  }
  return (((indexHigh - indexLow) / (breakpointHigh - breakpointLow)) * (ppm25 - breakpointLow) + indexLow).toDouble();
}

uint16_t get_ppm_10_AQI(float16 ppm10) {

  float16 indexHigh, indexLow;
  float16 breakpointHigh, breakpointLow;

  if (ppm10 > 0 && ppm10 < 54) {
    indexHigh = 50;
    indexLow = 0;
    breakpointHigh = 54;
    breakpointLow = 0;
  } else if (ppm10 > 55 && ppm10 < 154) {
    indexHigh = 100;
    indexLow = 51;
    breakpointHigh = 154;
    breakpointLow = 55;
  } else if (ppm10 > 155 && ppm10 < 254) {
    indexHigh = 150;
    indexLow = 101;
    breakpointHigh = 254;
    breakpointLow = 155;
  } else if (ppm10 > 255 && ppm10 < 354) {
    indexHigh = 151;
    indexLow = 200;
    breakpointHigh = 354;
    breakpointLow = 255;
  } else if (ppm10 > 355 && ppm10 < 424) {
    indexHigh = 201;
    indexLow = 300;
    breakpointHigh = 424;
    breakpointLow = 355;
  } else if (ppm10 > 425 && ppm10 < 504) {
    indexHigh = 400;
    indexLow = 301;
    breakpointHigh = 504;
    breakpointLow = 425;
  } else if (ppm10 > 505 && ppm10 < 604) {
    indexHigh = 500;
    indexLow = 401;
    breakpointHigh = 604;
    breakpointLow = 505;
  }
  return (((indexHigh - indexLow) / (breakpointHigh - breakpointLow)) * (ppm10 - breakpointLow) + indexLow).toDouble();
}

uint16_t get_CO_AQI(float16 CO) {


  float16 indexHigh, indexLow;
  float16 breakpointHigh, breakpointLow;

  if (CO > 0 && CO < 4.4) {
    indexHigh = 50;
    indexLow = 0;
    breakpointHigh = 4.4;
    breakpointLow = 0;
  } else if (CO > 4.5 && CO < 9.4) {
    indexHigh = 100;
    indexLow = 51;
    breakpointHigh = 9.4;
    breakpointLow = 4.5;
  } else if (CO > 9.5 && CO < 12.4) {
    indexHigh = 150;
    indexLow = 101;
    breakpointHigh = 12.4;
    breakpointLow = 9.5;
  } else if (CO > 12.5 && CO < 15.4) {
    indexHigh = 151;
    indexLow = 200;
    breakpointHigh = 15.4;
    breakpointLow = 12.5;
  } else if (CO > 15.5 && CO < 30.4) {
    indexHigh = 201;
    indexLow = 300;
    breakpointHigh = 30.4;
    breakpointLow = 15.5;
  } else if (CO > 30.5 && CO < 40.4) {
    indexHigh = 400;
    indexLow = 301;
    breakpointHigh = 40.4;
    breakpointLow = 30.5;
  } else if (CO > 40.5 && CO < 50.4) {
    indexHigh = 500;
    indexLow = 401;
    breakpointHigh = 50.4;
    breakpointLow = 40.5;
  }
  return (((indexHigh - indexLow) / (breakpointHigh - breakpointLow)) * (CO - breakpointLow) + indexLow).toDouble();
}

uint16_t get_composite_AQI(char* source, float16 ppm25, float16 ppm10, float16 CO) {
  uint16_t AQI25, AQI10, AQICO;
  AQI25 = get_ppm_25_AQI(ppm25);
  AQI10 = get_ppm_10_AQI(ppm10);
  AQICO = get_CO_AQI(CO);

  if (AQI25 > AQI10 && AQI25 > AQICO) {
    source = "25";
    return AQI25;
  } else if (AQI10 > AQI25 && AQI10 > AQICO) {
    source = "10";
    return AQI10;
  } else {
    source = "CO";
    return AQICO;
  }
}