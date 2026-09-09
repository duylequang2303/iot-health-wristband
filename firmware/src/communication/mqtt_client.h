#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../config.h"
#include "../processing/classifier.h"

/**
 * MQTT client for publishing sensor data and alerts.
 * Handles WiFi connection and MQTT broker communication.
 */
class MqttClient {
public:
    MqttClient();

    /**
     * Connect to WiFi and MQTT broker.
     * Returns true if both connections succeed.
     */
    bool begin();

    /**
     * Maintain connection (call in loop).
     * Reconnects if disconnected.
     */
    void loop();

    /**
     * Publish vitals data as JSON.
     */
    bool publishVitals(
        float hr, float spo2, float temp, float accelMag,
        const ClassificationResult& classification
    );

    /**
     * Publish alert when level >= WARNING.
     */
    bool publishAlert(
        const ClassificationResult& classification
    );

    bool isConnected();

private:
    WiFiClient   _wifiClient;
    PubSubClient _mqttClient;

    bool connectWiFi();
    bool connectMQTT();
};

#endif // MQTT_CLIENT_H
