#ifndef CONFIG_H
#define CONFIG_H

// ──────────────────────────────────────────────
// WiFi Configuration
// ──────────────────────────────────────────────
#define WIFI_SSID       "your-wifi-ssid"
#define WIFI_PASSWORD   "your-wifi-password"

// ──────────────────────────────────────────────
// MQTT Configuration (HiveMQ Cloud)
// ──────────────────────────────────────────────
#define MQTT_HOST       "your-cluster.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883
#define MQTT_USERNAME   "your_username"
#define MQTT_PASSWORD   "your_mqtt_password"
#define MQTT_CLIENT_ID  "esp32_health_wristband"

// MQTT Topics
#define USER_ID         "user_001"
#define TOPIC_VITALS    "health/" USER_ID "/vitals"
#define TOPIC_ALERTS    "health/" USER_ID "/alerts"
#define TOPIC_CONFIG    "health/" USER_ID "/config"

// ──────────────────────────────────────────────
// Sensor Sampling
// ──────────────────────────────────────────────
#define SAMPLE_INTERVAL_MS      3000    // Read sensors every 3 seconds
#define MQTT_PUBLISH_INTERVAL_MS 3000   // Publish MQTT every 3 seconds

// ──────────────────────────────────────────────
// Moving Average Filter Window Sizes
// ──────────────────────────────────────────────
#define FILTER_WINDOW_HR        10  // Heart rate – high variability
#define FILTER_WINDOW_SPO2      8   // SpO2 – medium variability
#define FILTER_WINDOW_TEMP      5   // Temperature – slow change
#define FILTER_WINDOW_ACCEL     15  // Accelerometer – very noisy

// ──────────────────────────────────────────────
// Baseline Algorithm – 2 Phase
// ──────────────────────────────────────────────
#define BASELINE_ALPHA_START    0.15f   // Fast learning in warm-up
#define BASELINE_ALPHA_END      0.01f   // Slow update in stable phase
#define BASELINE_WARMUP_SAMPLES 1000    // ~3 days at 3s intervals, 8h/day

// ──────────────────────────────────────────────
// Activity Detection (MPU6050)
// ──────────────────────────────────────────────
#define ACTIVITY_THRESHOLD      2000.0f // Magnitude threshold: REST < this < ACTIVE

// ──────────────────────────────────────────────
// Z-score Classification Thresholds (base)
// ──────────────────────────────────────────────
#define Z_NORMAL        1.0f    // |z| < 1.0 → NORMAL
#define Z_ATTENTION     2.0f    // |z| < 2.0 → ATTENTION
#define Z_WARNING       3.0f    // |z| < 3.0 → WARNING
                                // |z| >= 3.0 → CRITICAL

// ──────────────────────────────────────────────
// Absolute Safety Limits (failsafe, regardless of baseline)
// ──────────────────────────────────────────────
#define HR_ABS_MIN      30      // bpm
#define HR_ABS_MAX      220     // bpm
#define SPO2_ABS_MIN    70      // %
#define TEMP_ABS_MIN    34.0f   // °C
#define TEMP_ABS_MAX    42.0f   // °C

// ──────────────────────────────────────────────
// Hardware Pins
// ──────────────────────────────────────────────
// I2C (OLED + sensors)
#define PIN_SDA         21
#define PIN_SCL         22

// Vibration motor (or LED in simulation)
#define PIN_MOTOR       23

// Demo controls (Wokwi)
#define PIN_BTN_EMERGENCY   25
#define PIN_BTN_EXERCISE    26
#define PIN_POT_HR          34  // ADC input

// ──────────────────────────────────────────────
// OLED Display
// ──────────────────────────────────────────────
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_ADDRESS    0x3C

// ──────────────────────────────────────────────
// Vibration Patterns (duration in ms)
// ──────────────────────────────────────────────
#define VIBRATE_ATTENTION_MS    0       // No vibration for ATTENTION
#define VIBRATE_WARNING_MS      200     // Short buzz × 2
#define VIBRATE_WARNING_PAUSE   200     // Pause between buzzes
#define VIBRATE_CRITICAL_MS     1000    // Continuous vibration

// ──────────────────────────────────────────────
// Simulated Sensor Ranges (for Wokwi, no real sensors)
// ──────────────────────────────────────────────
#define SIM_HR_BASE         72.0f   // Normal resting HR
#define SIM_HR_NOISE        3.0f    // Random noise amplitude
#define SIM_SPO2_BASE       97.5f
#define SIM_SPO2_NOISE      0.8f
#define SIM_TEMP_BASE       36.6f
#define SIM_TEMP_NOISE      0.15f
#define SIM_ACCEL_REST      980.0f  // ~1g in rest
#define SIM_ACCEL_ACTIVE    3500.0f // Active movement

#endif // CONFIG_H
