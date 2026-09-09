#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>
#include "../config.h"

/**
 * Simulated MPU6050 accelerometer + gyroscope.
 * Reports acceleration magnitude for activity detection.
 */
class MotionSensor {
public:
    void begin();

    /**
     * Read acceleration magnitude (sqrt(ax² + ay² + az²)).
     * If exerciseButtonPressed, simulates high activity.
     */
    float readAccelMagnitude(bool exerciseButtonPressed = false);

private:
    unsigned long _startTime;
};

#endif // MOTION_H
