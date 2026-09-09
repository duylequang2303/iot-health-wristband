/**
 * Backend Server Entry Point
 * 
 * Connects to HiveMQ Cloud MQTT broker, subscribes to sensor data topics,
 * validates incoming data, and writes to Firebase Realtime Database.
 */

require('dotenv').config();
const { initializeFirebase } = require('./firebase/config');
const { MqttSubscriber } = require('./mqtt/subscriber');
const { DataProcessor } = require('./services/data-processor');
const { AlertService } = require('./services/alert-service');

async function main() {
    console.log('=== IoT Health Wristband Backend ===\n');

    // 1. Initialize Firebase
    const db = initializeFirebase();
    console.log('[Firebase] Connected\n');

    // 2. Initialize services
    const alertService = new AlertService(db);
    const dataProcessor = new DataProcessor(db, alertService);

    // 3. Start MQTT subscriber
    const subscriber = new MqttSubscriber(dataProcessor);
    subscriber.connect();

    // Graceful shutdown
    process.on('SIGINT', () => {
        console.log('\n[Server] Shutting down...');
        subscriber.disconnect();
        process.exit(0);
    });
}

main().catch(err => {
    console.error('[Fatal Error]', err);
    process.exit(1);
});
