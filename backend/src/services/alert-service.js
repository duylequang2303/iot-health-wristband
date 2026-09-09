/**
 * Alert Service
 * 
 * Handles creating and managing alerts in Firebase.
 */

const { Database } = require('../firebase/database');

class AlertService {
    constructor(db) {
        this.database = new Database(db);
    }

    /**
     * Create a new alert in Firebase.
     */
    async createAlert(userId, alertData) {
        const alert = {
            level: alertData.level,
            trigger_metrics: alertData.trigger_metrics || {},
            overall_level: alertData.level,
            message: this.generateMessage(alertData),
            timestamp: alertData.timestamp || Date.now()
        };

        return await this.database.writeAlert(userId, alert);
    }

    /**
     * Generate a human-readable alert message in Vietnamese.
     */
    generateMessage(alertData) {
        const triggers = alertData.trigger_metrics || {};
        const parts = [];

        if (triggers.heart_rate) {
            const hr = triggers.heart_rate;
            parts.push(`Nhịp tim ${hr.value} bpm (z=${hr.z_score}, trạng thái ${hr.state || 'N/A'})`);
        }

        if (triggers.spo2) {
            const spo2 = triggers.spo2;
            parts.push(`SpO2 ${spo2.value}% (z=${spo2.z_score})`);
        }

        if (triggers.temperature) {
            const temp = triggers.temperature;
            parts.push(`Nhiệt độ ${temp.value}°C (z=${temp.z_score})`);
        }

        if (parts.length === 0) {
            return `Cảnh báo mức ${alertData.level}`;
        }

        const levelVi = {
            'warning': 'Cảnh báo',
            'critical': 'Khẩn cấp'
        };

        return `${levelVi[alertData.level] || alertData.level}: ${parts.join(', ')}`;
    }
}

module.exports = { AlertService };
