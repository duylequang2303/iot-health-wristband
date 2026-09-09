#include "temperature.h"
#include <math.h>

void TemperatureSensor::begin() {
    _startTime = millis();
}

float TemperatureSensor::readTemperature() {
    float t = (float)(millis() - _startTime) / 1000.0f;
    // Very slow drift (body temp is stable)
    float variation = sin(t * 0.01f) * 0.1f;
    float noise = (random(-100, 101) / 100.0f) * SIM_TEMP_NOISE;
    return SIM_TEMP_BASE + variation + noise;
}
