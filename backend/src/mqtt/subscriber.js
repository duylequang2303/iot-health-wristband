/**
 * MQTT Subscriber
 * 
 * Connects to HiveMQ Cloud and subscribes to health data topics.
 */

const mqtt = require('mqtt');

class MqttSubscriber {
    constructor(dataProcessor) {
        this.dataProcessor = dataProcessor;
        this.client = null;
    }

    connect() {
        const { MQTT_HOST, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD } = process.env;

        if (!MQTT_HOST) {
            throw new Error('MQTT_HOST is not set in .env');
        }

        const url = `mqtts://${MQTT_HOST}:${MQTT_PORT || 8883}`;
        console.log(`[MQTT] Connecting to ${url}...`);

        this.client = mqtt.connect(url, {
            username: MQTT_USERNAME,
            password: MQTT_PASSWORD,
            rejectUnauthorized: true,
            reconnectPeriod: 5000,
        });

        this.client.on('connect', () => {
            console.log('[MQTT] Connected to broker');

            // Subscribe to all user vitals and alerts topics
            this.client.subscribe('health/+/vitals', { qos: 1 }, (err) => {
                if (err) console.error('[MQTT] Subscribe error (vitals):', err);
                else console.log('[MQTT] Subscribed to health/+/vitals');
            });

            this.client.subscribe('health/+/alerts', { qos: 1 }, (err) => {
                if (err) console.error('[MQTT] Subscribe error (alerts):', err);
                else console.log('[MQTT] Subscribed to health/+/alerts');
            });
        });

        this.client.on('message', (topic, message) => {
            try {
                const data = JSON.parse(message.toString());
                const parts = topic.split('/');
                const userId = parts[1];
                const type = parts[2]; // 'vitals' or 'alerts'

                if (type === 'vitals') {
                    this.dataProcessor.processVitals(userId, data);
                } else if (type === 'alerts') {
                    this.dataProcessor.processAlert(userId, data);
                }
            } catch (err) {
                console.error('[MQTT] Error processing message:', err.message);
            }
        });

        this.client.on('error', (err) => {
            console.error('[MQTT] Error:', err.message);
        });

        this.client.on('reconnect', () => {
            console.log('[MQTT] Reconnecting...');
        });

        this.client.on('close', () => {
            console.log('[MQTT] Connection closed');
        });
    }

    disconnect() {
        if (this.client) {
            this.client.end();
            console.log('[MQTT] Disconnected');
        }
    }
}

module.exports = { MqttSubscriber };
