/**
 * Mock MQTT Publisher
 * 
 * Simulates ESP32 firmware by publishing fake sensor data to MQTT broker.
 * Use this to test backend independently while firmware is in development.
 * 
 * Usage: npm run mock
 */

require('dotenv').config();
const mqtt = require('mqtt');

const USER_ID = 'user_001';
const TOPIC_VITALS = `health/${USER_ID}/vitals`;
const TOPIC_ALERTS = `health/${USER_ID}/alerts`;
const PUBLISH_INTERVAL = 3000; // 3 seconds

// Simulated baseline
const baseline = { hr: 72, spo2: 97.5, temp: 36.6 };

function randomInRange(base, noise) {
    return base + (Math.random() - 0.5) * 2 * noise;
}

function classifyZ(z) {
    if (Math.abs(z) < 1.0) return 'normal';
    if (Math.abs(z) < 2.0) return 'attention';
    if (Math.abs(z) < 3.0) return 'warning';
    return 'critical';
}

function generateVitals() {
    const hr = randomInRange(baseline.hr, 5);
    const spo2 = Math.min(100, randomInRange(baseline.spo2, 1.5));
    const temp = randomInRange(baseline.temp, 0.3);
    const accel = randomInRange(980, 100);
    const state = accel > 2000 ? 'active' : 'rest';

    const zHR = Math.abs(hr - baseline.hr) / 8;
    const zSpO2 = Math.abs(spo2 - baseline.spo2) / 1.2;
    const zTemp = Math.abs(temp - baseline.temp) / 0.3;

    const hrLevel = classifyZ(zHR);
    const spo2Level = classifyZ(zSpO2);
    const tempLevel = classifyZ(zTemp);
    const levels = [hrLevel, spo2Level, tempLevel];
    const levelOrder = ['normal', 'attention', 'warning', 'critical'];
    const overall = levels.reduce((max, l) =>
        levelOrder.indexOf(l) > levelOrder.indexOf(max) ? l : max, 'normal');

    return {
        user_id: USER_ID,
        heart_rate: hr.toFixed(1),
        spo2: spo2.toFixed(1),
        temperature: temp.toFixed(1),
        accel_magnitude: accel.toFixed(0),
        activity_state: state,
        classifications: {
            heart_rate: hrLevel,
            spo2: spo2Level,
            temperature: tempLevel
        },
        overall: overall,
        baseline_phase: 'stable',
        timestamp: Date.now()
    };
}

// ── Connect and publish ──
const url = `mqtts://${process.env.MQTT_HOST}:${process.env.MQTT_PORT || 8883}`;
console.log(`[Mock] Connecting to ${url}...`);

const client = mqtt.connect(url, {
    username: process.env.MQTT_USERNAME,
    password: process.env.MQTT_PASSWORD,
    rejectUnauthorized: true,
});

client.on('connect', () => {
    console.log('[Mock] Connected! Publishing every 3 seconds...\n');

    setInterval(() => {
        const vitals = generateVitals();

        client.publish(TOPIC_VITALS, JSON.stringify(vitals), { qos: 1 });
        console.log(`[Mock] HR=${vitals.heart_rate} SpO2=${vitals.spo2} ` +
            `Temp=${vitals.temperature} → ${vitals.overall}`);

        // Also publish alert if WARNING or CRITICAL
        if (vitals.overall === 'warning' || vitals.overall === 'critical') {
            const alert = {
                user_id: USER_ID,
                level: vitals.overall,
                trigger_metrics: {},
                timestamp: Date.now()
            };

            if (vitals.classifications.heart_rate !== 'normal') {
                alert.trigger_metrics.heart_rate = {
                    value: parseFloat(vitals.heart_rate),
                    z_score: '2.5',
                    level: vitals.classifications.heart_rate,
                    state: vitals.activity_state
                };
            }

            client.publish(TOPIC_ALERTS, JSON.stringify(alert), { qos: 1 });
            console.log(`[Mock] 🚨 Alert: ${vitals.overall.toUpperCase()}`);
        }
    }, PUBLISH_INTERVAL);
});

client.on('error', (err) => {
    console.error('[Mock] Error:', err.message);
});

process.on('SIGINT', () => {
    client.end();
    console.log('\n[Mock] Stopped');
    process.exit(0);
});
