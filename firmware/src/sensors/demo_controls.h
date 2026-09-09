#ifndef DEMO_CONTROLS_H
#define DEMO_CONTROLS_H

#include <Arduino.h>
#include "../config.h"

/**
 * Demo controls for Wokwi simulation:
 * - Emergency button: triggers CRITICAL scenario
 * - Exercise button: simulates active movement
 * - HR potentiometer: manually override heart rate
 */
class DemoControls {
public:
    void begin();

    bool isEmergencyPressed();
    bool isExercisePressed();

    /**
     * Read potentiometer value. Returns raw ADC (0–4095).
     * Returns -1 if potentiometer is at minimum (no override).
     */
    int readHRPotentiometer();
};

#endif // DEMO_CONTROLS_H
