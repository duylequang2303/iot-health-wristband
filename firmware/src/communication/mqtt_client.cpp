#include "mqtt_client.h"

MqttClient::MqttClient() : _mqttClient(_wifiClient) {}

bool MqttClient::begin() {
    if (!connectWiFi()) return false;
    if (!connectMQTT()) return false;
    return true;
}

bool MqttClient::connectWiFi() {
    Serial.print(F("[WiFi] Connecting to "));
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print(F("[WiFi] Connected! IP: "));
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println(F("\n[WiFi] Connection failed!"));
        return false;
    }
}

bool MqttClient::connectMQTT() {
    _mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    _mqttClient.setBufferSize(1024);  // Increase buffer for JSON payloads

    Serial.print(F("[MQTT] Connecting to "));
    Serial.println(MQTT_HOST);

    int attempts = 0;
    while (!_mqttClient.connected() && attempts < 5) {
        if (_mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println(F("[MQTT] Connected!"));
            // Subscribe to config topic for receiving threshold updates
            _mqttClient.subscribe(TOPIC_CONFIG);
            return true;
        } else {
            Serial.print(F("[MQTT] Failed, rc="));
            Serial.print(_mqttClient.state());
            Serial.println(F(". Retrying..."));
            delay(2000);
            attempts++;
        }
    }
    return false;
}

void MqttClient::loop() {
    if (!_mqttClient.connected()) {
        connectMQTT();
    }
    _mqttClient.loop();
}

bool MqttClient::isConnected() {
    return _mqttClient.connected();
}

bool MqttClient::publishVitals(
    float hr, float spo2, float temp, float accelMag,
    const ClassificationResult& classification
) {
    JsonDocument doc;

    doc["user_id"] = USER_ID;
    doc["heart_rate"] = serialized(String(hr, 1));
    doc["spo2"] = serialized(String(spo2, 1));
    doc["temperature"] = serialized(String(temp, 1));
    doc["accel_magnitude"] = serialized(String(accelMag, 0));
    doc["activity_state"] = Classifier::stateToString(classification.activityState);

    JsonObject classObj = doc["classifications"].to<JsonObject>();
    classObj["heart_rate"] = Classifier::levelToString(classification.heartRate.level);
    classObj["spo2"] = Classifier::levelToString(classification.spo2.level);
    classObj["temperature"] = Classifier::levelToString(classification.temperature.level);

    doc["overall"] = Classifier::levelToString(classification.overall);
    doc["baseline_phase"] = Classifier::phaseToString(classification.baselinePhase);
    doc["timestamp"] = millis();

    char buffer[512];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));

    bool success = _mqttClient.publish(TOPIC_VITALS, buffer, len);
    if (success) {
        Serial.print(F("[MQTT] Published vitals: "));
        Serial.println(buffer);
    } else {
        Serial.println(F("[MQTT] Failed to publish vitals"));
    }
    return success;
}

bool MqttClient::publishAlert(const ClassificationResult& classification) {
    if (classification.overall < LEVEL_WARNING) {
        return false;  // Only publish WARNING and CRITICAL
    }

    JsonDocument doc;

    doc["user_id"] = USER_ID;
    doc["level"] = Classifier::levelToString(classification.overall);

    // Report which metric(s) triggered the alert
    JsonObject triggers = doc["trigger_metrics"].to<JsonObject>();

    if (classification.heartRate.level >= LEVEL_WARNING) {
        JsonObject hrTrig = triggers["heart_rate"].to<JsonObject>();
        hrTrig["value"] = classification.heartRate.value;
        hrTrig["z_score"] = serialized(String(classification.heartRate.zScore, 2));
        hrTrig["level"] = Classifier::levelToString(classification.heartRate.level);
        hrTrig["state"] = Classifier::stateToString(classification.activityState);
    }

    if (classification.spo2.level >= LEVEL_WARNING) {
        JsonObject spo2Trig = triggers["spo2"].to<JsonObject>();
        spo2Trig["value"] = classification.spo2.value;
        spo2Trig["z_score"] = serialized(String(classification.spo2.zScore, 2));
        spo2Trig["level"] = Classifier::levelToString(classification.spo2.level);
    }

    if (classification.temperature.level >= LEVEL_WARNING) {
        JsonObject tempTrig = triggers["temperature"].to<JsonObject>();
        tempTrig["value"] = classification.temperature.value;
        tempTrig["z_score"] = serialized(String(classification.temperature.zScore, 2));
        tempTrig["level"] = Classifier::levelToString(classification.temperature.level);
    }

    doc["timestamp"] = millis();

    char buffer[512];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));

    bool success = _mqttClient.publish(TOPIC_ALERTS, buffer, len);
    if (success) {
        Serial.print(F("[MQTT] Published alert: "));
        Serial.println(buffer);
    }
    return success;
}
