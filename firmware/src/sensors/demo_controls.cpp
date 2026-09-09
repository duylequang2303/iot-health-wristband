#include "demo_controls.h"

void DemoControls::begin() {
    pinMode(PIN_BTN_EMERGENCY, INPUT_PULLUP);
    pinMode(PIN_BTN_EXERCISE, INPUT_PULLUP);
    // ADC pin doesn't need explicit setup on ESP32
}

bool DemoControls::isEmergencyPressed() {
    return digitalRead(PIN_BTN_EMERGENCY) == LOW;  // Active low with pull-up
}

bool DemoControls::isExercisePressed() {
    return digitalRead(PIN_BTN_EXERCISE) == LOW;
}

int DemoControls::readHRPotentiometer() {
    int raw = analogRead(PIN_POT_HR);
    // If pot is near minimum (< 50), return -1 = no override
    if (raw < 50) return -1;
    return raw;
}
