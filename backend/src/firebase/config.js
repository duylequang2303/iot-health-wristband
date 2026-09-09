/**
 * Firebase Admin SDK Configuration
 */

const admin = require('firebase-admin');
const path = require('path');

function initializeFirebase() {
    const serviceAccountPath = process.env.FIREBASE_SERVICE_ACCOUNT_PATH || './serviceAccountKey.json';
    const databaseURL = process.env.FIREBASE_DATABASE_URL;

    if (!databaseURL) {
        throw new Error('FIREBASE_DATABASE_URL is not set in .env');
    }

    try {
        const serviceAccount = require(path.resolve(serviceAccountPath));
        admin.initializeApp({
            credential: admin.credential.cert(serviceAccount),
            databaseURL: databaseURL
        });
    } catch (err) {
        console.warn('[Firebase] Service account not found, using default credentials');
        admin.initializeApp({
            databaseURL: databaseURL
        });
    }

    return admin.database();
}

module.exports = { initializeFirebase };
