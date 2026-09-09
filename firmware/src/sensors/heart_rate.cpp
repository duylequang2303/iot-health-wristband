#include "heart_rate.h"
#include <math.h>

void HeartRateSensor::begin() {
    _startTime = millis();
}

float HeartRateSensor::readHeartRate(int potOverrideValue) {
    // If pot override is active (0–4095 from ADC), map to HR range 40–200
    if (potOverrideValue >= 0) {
        return 40.0f + ((float)potOverrideValue / 4095.0f) * 160.0f;
    }

    // Simulated HR: base + slow sine variation + random noise
    float t = (float)(millis() - _startTime) / 1000.0f;
    float slowVariation = sin(t * 0.05f) * 5.0f;   // Slow drift ±5 BPM
    float noise = (random(-100, 101) / 100.0f) * SIM_HR_NOISE;
    return SIM_HR_BASE + slowVariation + noise;
}

float HeartRateSensor::readSpO2() {
    // Simulated SpO2: base + small variation + noise
    float t = (float)(millis() - _startTime) / 1000.0f;
    float variation = sin(t * 0.03f) * 0.5f;  // Slow drift ±0.5%
    float noise = (random(-100, 101) / 100.0f) * SIM_SPO2_NOISE;
    float spo2 = SIM_SPO2_BASE + variation + noise;
    // Clamp to valid range
    if (spo2 > 100.0f) spo2 = 100.0f;
    if (spo2 < 70.0f) spo2 = 70.0f;
    return spo2;
}
