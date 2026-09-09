/**
 * Data Processor
 * 
 * Validates and transforms incoming MQTT data, then writes to Firebase.
 */

const { Database } = require('../firebase/database');

class DataProcessor {
    constructor(db, alertService) {
        this.database = new Database(db);
        this.alertService = alertService;
        this.messageCount = 0;
    }

    /**
     * Process incoming vitals data from MQTT.
     */
    async processVitals(userId, data) {
        try {
            // Validate required fields
            if (!this.validateVitals(data)) {
                console.warn(`[Processor] Invalid vitals data from ${userId}:`, data);
                return;
            }

            // Write sensor data to Firebase
            const key = await this.database.writeSensorData(userId, {
                heart_rate: parseFloat(data.heart_rate),
                spo2: parseFloat(data.spo2),
                temperature: parseFloat(data.temperature),
                accel_magnitude: parseFloat(data.accel_magnitude),
                activity_state: data.activity_state,
                classifications: data.classifications,
                overall_classification: data.overall,
                baseline_phase: data.baseline_phase,
                timestamp: data.timestamp || Date.now()
            });

            this.messageCount++;
            if (this.messageCount % 10 === 0) {
                console.log(`[Processor] Processed ${this.messageCount} messages. ` +
                    `Latest: HR=${data.heart_rate} SpO2=${data.spo2} Temp=${data.temperature} → ${data.overall}`);
            }

            // Update baseline phase info
            await this.database.updateBaselinePhase(userId, data.baseline_phase, data.timestamp);

        } catch (err) {
            console.error(`[Processor] Error processing vitals for ${userId}:`, err.message);
        }
    }

    /**
     * Process incoming alert from MQTT.
     */
    async processAlert(userId, data) {
        try {
            if (!data.level || data.level === 'normal' || data.level === 'attention') {
                return; // Only store WARNING and CRITICAL
            }

            await this.alertService.createAlert(userId, data);
            console.log(`[Processor] 🚨 Alert for ${userId}: ${data.level.toUpperCase()}`);

        } catch (err) {
            console.error(`[Processor] Error processing alert for ${userId}:`, err.message);
        }
    }

    /**
     * Validate vitals data has required fields.
     */
    validateVitals(data) {
        const required = ['heart_rate', 'spo2', 'temperature', 'activity_state', 'overall'];
        for (const field of required) {
            if (data[field] === undefined || data[field] === null) {
                return false;
            }
        }

        // Range checks
        const hr = parseFloat(data.heart_rate);
        const spo2 = parseFloat(data.spo2);
        const temp = parseFloat(data.temperature);

        if (isNaN(hr) || hr < 0 || hr > 300) return false;
        if (isNaN(spo2) || spo2 < 0 || spo2 > 100) return false;
        if (isNaN(temp) || temp < 20 || temp > 50) return false;

        return true;
    }
}

module.exports = { DataProcessor };
