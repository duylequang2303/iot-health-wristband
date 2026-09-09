#include "vibration.h"

void VibrationMotor::begin() {
    pinMode(PIN_MOTOR, OUTPUT);
    digitalWrite(PIN_MOTOR, LOW);
    _lastVibrationTime = 0;
    _vibrationStep = 0;
    _currentLevel = LEVEL_NORMAL;
}

void VibrationMotor::update(AlertLevel level) {
    _currentLevel = level;
    unsigned long now = millis();

    switch (level) {
        case LEVEL_NORMAL:
        case LEVEL_ATTENTION:
            // No vibration
            digitalWrite(PIN_MOTOR, LOW);
            _vibrationStep = 0;
            break;

        case LEVEL_WARNING:
            // Short buzz pattern: ON 200ms → OFF 200ms → ON 200ms → OFF (repeats)
            if (_vibrationStep == 0) {
                digitalWrite(PIN_MOTOR, HIGH);
                _lastVibrationTime = now;
                _vibrationStep = 1;
            } else if (_vibrationStep == 1 && now - _lastVibrationTime >= VIBRATE_WARNING_MS) {
                digitalWrite(PIN_MOTOR, LOW);
                _lastVibrationTime = now;
                _vibrationStep = 2;
            } else if (_vibrationStep == 2 && now - _lastVibrationTime >= VIBRATE_WARNING_PAUSE) {
                digitalWrite(PIN_MOTOR, HIGH);
                _lastVibrationTime = now;
                _vibrationStep = 3;
            } else if (_vibrationStep == 3 && now - _lastVibrationTime >= VIBRATE_WARNING_MS) {
                digitalWrite(PIN_MOTOR, LOW);
                _lastVibrationTime = now;
                _vibrationStep = 4;
            } else if (_vibrationStep == 4 && now - _lastVibrationTime >= 2000) {
                // Wait 2 seconds before repeating
                _vibrationStep = 0;
            }
            break;

        case LEVEL_CRITICAL:
            // Continuous vibration
            digitalWrite(PIN_MOTOR, HIGH);
            break;
    }
}
