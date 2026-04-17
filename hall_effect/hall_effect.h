#ifndef HALL_EFFECT_H_
#define HALL_EFFECT_H_

#include <stdbool.h>

void initHallEffect(); // Init GPI pull-down pins for both hall effect sensors
bool getFarSensorStatus(); // Poll state of far sensor
bool getNearSensorStatus(); // Poll state of near sensor

#endif

