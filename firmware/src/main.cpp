/**
 * Adaptive Vitals Monitor – ESP32 Firmware
 * 
 * Main entry point for the IoT health wristband system.
 * 
 * Pipeline: Sensors → Filter → Baseline → Classify → Display/Vibrate → MQTT Publish
 * 
 * @author Nhóm 3 – Đề tài 31
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

// Sensors
#include "sensors/heart_rate.h"
#include "sensors/temperature.h"
#include "sensors/motion.h"
#include "sensors/demo_controls.h"

// Processing
#include "processing/filter.h"
#include "processing/baseline.h"
#include "processing/classifier.h"

// Output
#include "display/oled_display.h"
#include "display/vibration.h"

// Communication
#include "communication/mqtt_client.h"

// ──────────────────────────────────────────────
// Global instances
// ──────────────────────────────────────────────
HeartRateSensor   heartRateSensor;
TemperatureSensor tempSensor;
MotionSensor      motionSensor;
DemoControls      demoControls;

MovingAverageFilter<20> filterHR;
MovingAverageFilter<20> filterSpO2;
MovingAverageFilter<20> filterTemp;
MovingAverageFilter<20> filterAccel;

BaselineManager   baselineManager;
Classifier        classifier;

OledDisplay       oledDisplay;
VibrationMotor    vibrationMotor;
MqttClient        mqttClient;

// Timing
unsigned long lastSampleTime = 0;
unsigned long lastPublishTime = 0;

// ──────────────────────────────────────────────
// SETUP
// ──────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Serial.println(F("\n=== Adaptive Vitals Monitor ==="));
    Serial.println(F("Firmware starting..."));

    // Initialize I2C
    Wire.begin(PIN_SDA, PIN_SCL);

    // Initialize sensors
    heartRateSensor.begin();
    tempSensor.begin();
    motionSensor.begin();
    demoControls.begin();
    Serial.println(F("[Sensors] Initialized (simulation mode)"));

    // Initialize filters
    filterHR.begin(FILTER_WINDOW_HR);
    filterSpO2.begin(FILTER_WINDOW_SPO2);
    filterTemp.begin(FILTER_WINDOW_TEMP);
    filterAccel.begin(FILTER_WINDOW_ACCEL);
    Serial.println(F("[Filters] Initialized"));

    // Initialize baseline manager
    baselineManager.begin();
    Serial.println(F("[Baseline] Initialized (2-phase, activity-aware)"));

    // Initialize classifier (no conditions by default)
    classifier.setConditions(Classifier::COND_NONE);
    Serial.println(F("[Classifier] Initialized (no conditions)"));

    // Initialize display
    if (oledDisplay.begin()) {
        Serial.println(F("[OLED] Initialized"));
    }

    // Initialize vibration motor
    vibrationMotor.begin();
    Serial.println(F("[Motor] Initialized"));

    // Connect WiFi + MQTT
    Serial.println(F("[Network] Connecting..."));
    if (mqttClient.begin()) {
        Serial.println(F("[Network] WiFi + MQTT connected!"));
    } else {
        Serial.println(F("[Network] Connection failed - will retry in loop"));
    }

    Serial.println(F("\n=== System Ready ==="));
    Serial.printf("[Baseline] Phase: WARMUP (need %d samples)\n", BASELINE_WARMUP_SAMPLES);
    Serial.println();
}

// ──────────────────────────────────────────────
// LOOP
// ──────────────────────────────────────────────
void loop() {
    unsigned long now = millis();

    // Maintain MQTT connection
    mqttClient.loop();

    // ── Sample sensors at fixed interval ──
    if (now - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = now;

        // ─── 1. Read demo controls ───
        bool emergencyPressed = demoControls.isEmergencyPressed();
        bool exercisePressed = demoControls.isExercisePressed();
        int potValue = demoControls.readHRPotentiometer();

        // ─── 2. Read sensors (simulated) ───
        float rawHR, rawSpO2, rawTemp, rawAccel;

        if (emergencyPressed) {
            // Emergency override — extreme values
            rawHR = 180.0f;
            rawSpO2 = 85.0f;
            rawTemp = 39.5f;
            rawAccel = SIM_ACCEL_REST;
        } else {
            rawHR = heartRateSensor.readHeartRate(potValue);
            rawSpO2 = heartRateSensor.readSpO2();
            rawTemp = tempSensor.readTemperature();
            rawAccel = motionSensor.readAccelMagnitude(exercisePressed);

            // If exercising, HR naturally increases
            if (exercisePressed) {
                rawHR += 40.0f;  // Simulate exercise-induced HR increase
            }
        }

        // ─── 3. Filter ───
        float filteredHR = filterHR.addSample(rawHR);
        float filteredSpO2 = filterSpO2.addSample(rawSpO2);
        float filteredTemp = filterTemp.addSample(rawTemp);
        float filteredAccel = filterAccel.addSample(rawAccel);

        // ─── 4. Determine activity state ───
        ActivityState activityState = BaselineManager::classifyActivity(filteredAccel);

        // ─── 5. Update baseline ───
        float alpha = baselineManager.update(activityState, filteredHR, filteredSpO2, filteredTemp);
        BaselinePhase phase = baselineManager.getPhase();
        const StateBaseline& currentBaseline = baselineManager.getBaseline(activityState);

        // ─── 6. Classify ───
        ClassificationResult classification = classifier.classify(
            filteredHR, filteredSpO2, filteredTemp,
            activityState, phase, currentBaseline
        );

        // ─── 7. Update display ───
        oledDisplay.update(filteredHR, filteredSpO2, filteredTemp, classification);

        // ─── 8. Update vibration motor ───
        vibrationMotor.update(classification.overall);

        // ─── 9. Serial output for debugging ───
        Serial.printf("[%lums] HR=%.1f SpO2=%.1f Temp=%.1f Accel=%.0f | "
                      "State=%s Phase=%s Alpha=%.4f | "
                      "HR:%s SpO2:%s Temp:%s → Overall:%s\n",
            now,
            filteredHR, filteredSpO2, filteredTemp, filteredAccel,
            Classifier::stateToString(activityState),
            Classifier::phaseToString(phase),
            alpha,
            Classifier::levelToString(classification.heartRate.level),
            Classifier::levelToString(classification.spo2.level),
            Classifier::levelToString(classification.temperature.level),
            Classifier::levelToString(classification.overall)
        );

        // ─── 10. Publish MQTT ───
        if (now - lastPublishTime >= MQTT_PUBLISH_INTERVAL_MS && mqttClient.isConnected()) {
            lastPublishTime = now;
            mqttClient.publishVitals(filteredHR, filteredSpO2, filteredTemp,
                                      filteredAccel, classification);
            mqttClient.publishAlert(classification);
        }
    }

    // Keep vibration motor updated (non-blocking patterns)
    vibrationMotor.update(vibrationMotor._currentLevel);
}
