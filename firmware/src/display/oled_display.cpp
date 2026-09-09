#include "oled_display.h"

bool OledDisplay::begin() {
    _display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println(F("[OLED] Failed to initialize SSD1306"));
        return false;
    }
    showSplash();
    return true;
}

void OledDisplay::showSplash() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    _display.setCursor(10, 10);
    _display.println(F("Adaptive Vitals"));
    _display.setCursor(30, 25);
    _display.println(F("Monitor"));
    _display.setCursor(15, 45);
    _display.println(F("Initializing..."));

    _display.display();
}

void OledDisplay::update(
    float hr, float spo2, float temp,
    const ClassificationResult& classification
) {
    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);

    // ── Row 0: Status bar ──
    _display.setTextSize(1);
    _display.setCursor(0, 0);

    // Show baseline phase
    if (classification.baselinePhase == PHASE_WARMUP) {
        _display.print(F("WARMING UP"));
    } else {
        // Show activity state
        if (classification.activityState == STATE_ACTIVE) {
            _display.print(F("ACTIVE"));
        } else {
            _display.print(F("REST"));
        }
    }

    // Show overall alert level on the right side
    const char* levelStr = Classifier::levelToString(classification.overall);
    int levelLen = strlen(levelStr);
    _display.setCursor(128 - (levelLen * 6), 0);
    _display.print(levelStr);

    // ── Row 1: Heart Rate ──
    _display.setTextSize(1);
    _display.setCursor(0, 14);
    _display.print(F("HR: "));
    _display.setTextSize(2);
    _display.print((int)hr);
    _display.setTextSize(1);
    _display.print(F(" bpm"));

    // Alert indicator for HR
    if (classification.heartRate.level >= LEVEL_WARNING) {
        _display.setCursor(110, 14);
        _display.print(F("!!"));
    } else if (classification.heartRate.level == LEVEL_ATTENTION) {
        _display.setCursor(116, 14);
        _display.print(F("!"));
    }

    // ── Row 2: SpO2 ──
    _display.setCursor(0, 34);
    _display.print(F("SpO2: "));
    _display.print(spo2, 1);
    _display.print(F("%"));

    if (classification.spo2.level >= LEVEL_WARNING) {
        _display.setCursor(110, 34);
        _display.print(F("!!"));
    }

    // ── Row 3: Temperature ──
    _display.setCursor(0, 46);
    _display.print(F("Temp: "));
    _display.print(temp, 1);
    _display.print(F("C"));

    if (classification.temperature.level >= LEVEL_WARNING) {
        _display.setCursor(110, 46);
        _display.print(F("!!"));
    }

    // ── Bottom bar: Alert level indicator ──
    drawAlertBar(classification.overall);

    _display.display();
}

void OledDisplay::drawAlertBar(AlertLevel level) {
    // Draw a bar at the bottom of the screen indicating alert level
    int barY = 60;
    int barHeight = 4;

    switch (level) {
        case LEVEL_NORMAL:
            // Thin line — all is well
            _display.drawFastHLine(0, barY + 2, 128, SSD1306_WHITE);
            break;
        case LEVEL_ATTENTION:
            // Half bar
            _display.fillRect(0, barY, 64, barHeight, SSD1306_WHITE);
            break;
        case LEVEL_WARNING:
            // 3/4 bar
            _display.fillRect(0, barY, 96, barHeight, SSD1306_WHITE);
            break;
        case LEVEL_CRITICAL:
            // Full bar + blinking effect (inverted every other second)
            if ((millis() / 500) % 2 == 0) {
                _display.fillRect(0, barY, 128, barHeight, SSD1306_WHITE);
            }
            break;
    }
}
