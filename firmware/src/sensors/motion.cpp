#include "motion.h"
#include <math.h>

void MotionSensor::begin() {
    _startTime = millis();
}

float MotionSensor::readAccelMagnitude(bool exerciseButtonPressed) {
    if (exerciseButtonPressed) {
        // Simulate active movement — high magnitude with variation
        float noise = (random(-200, 201) / 100.0f) * 300.0f;
        return SIM_ACCEL_ACTIVE + noise;
    }

    // Simulate resting — ~1g with small noise
    float noise = (random(-100, 101) / 100.0f) * 100.0f;
    return SIM_ACCEL_REST + noise;
}
