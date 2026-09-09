/**
 * Firebase Database Operations
 */

class Database {
    constructor(db) {
        this.db = db;
    }

    /**
     * Write sensor data to /sensor_data/{userId}
     */
    async writeSensorData(userId, data) {
        const ref = this.db.ref(`sensor_data/${userId}`);
        const newRef = ref.push();
        await newRef.set({
            ...data,
            server_timestamp: Date.now()
        });
        return newRef.key;
    }

    /**
     * Write alert to /alerts/{userId}
     */
    async writeAlert(userId, alertData) {
        const ref = this.db.ref(`alerts/${userId}`);
        const newRef = ref.push();
        await newRef.set({
            ...alertData,
            acknowledged: false,
            server_timestamp: Date.now()
        });
        return newRef.key;
    }

    /**
     * Update baseline for a user's activity state
     */
    async updateBaseline(userId, state, baselineData) {
        const ref = this.db.ref(`baselines/${userId}/${state}`);
        await ref.update(baselineData);
    }

    /**
     * Get user profile
     */
    async getUser(userId) {
        const snapshot = await this.db.ref(`users/${userId}`).once('value');
        return snapshot.val();
    }

    /**
     * Update baseline phase
     */
    async updateBaselinePhase(userId, phase, totalSamples) {
        const ref = this.db.ref(`baselines/${userId}`);
        await ref.update({
            phase: phase,
            total_samples: totalSamples,
            last_updated: new Date().toISOString()
        });
    }
}

module.exports = { Database };
