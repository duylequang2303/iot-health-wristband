#ifndef VIBRATION_H
#define VIBRATION_H

#include <Arduino.h>
#include "../config.h"
#include "../processing/classifier.h"

/**
 * Vibration motor controller.
 * In Wokwi simulation, uses an LED to indicate vibration.
 */
class VibrationMotor {
public:
    void begin();

    /**
     * Trigger vibration pattern based on alert level.
     * Non-blocking — uses millis() for timing.
     */
    void update(AlertLevel level);

    AlertLevel    _currentLevel;

private:
    unsigned long _lastVibrationTime;
    int           _vibrationStep;
};

#endif // VIBRATION_H
