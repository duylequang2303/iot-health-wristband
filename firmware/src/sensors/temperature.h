#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include "../config.h"

/**
 * Simulated MAX30205 body temperature sensor.
 */
class TemperatureSensor {
public:
    void begin();
    float readTemperature();

private:
    unsigned long _startTime;
};

#endif // TEMPERATURE_H
