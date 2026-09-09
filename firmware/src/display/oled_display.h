#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "../config.h"
#include "../processing/baseline.h"
#include "../processing/classifier.h"

/**
 * OLED display manager for SSD1306 128x64.
 * Shows vital signs, alert levels, and baseline phase.
 */
class OledDisplay {
public:
    bool begin();

    /**
     * Update the display with current readings and classification.
     */
    void update(
        float hr, float spo2, float temp,
        const ClassificationResult& classification
    );

    /**
     * Show startup/splash screen.
     */
    void showSplash();

private:
    Adafruit_SSD1306 _display;

    /**
     * Draw alert level indicator.
     */
    void drawAlertBar(AlertLevel level);
};

#endif // OLED_DISPLAY_H
