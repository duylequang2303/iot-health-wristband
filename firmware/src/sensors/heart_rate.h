#ifndef HEART_RATE_H
#define HEART_RATE_H

#include <Arduino.h>
#include "../config.h"

/**
 * Simulated MAX30102 sensor — generates realistic HR and SpO2 readings.
 * Uses sine waves + noise for simulation on Wokwi.
 */
class HeartRateSensor {
public:
    void begin();

    /**
     * Read simulated heart rate (BPM).
     * If potOverrideValue >= 0, use potentiometer to override HR.
     */
    float readHeartRate(int potOverrideValue = -1);

    /**
     * Read simulated SpO2 (%).
     */
    float readSpO2();

private:
    unsigned long _startTime;
};

#endif // HEART_RATE_H
