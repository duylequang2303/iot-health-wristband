#include "baseline.h"
#include <math.h>

BaselineManager::BaselineManager() : _totalSamples(0) {}

void BaselineManager::begin() {
    // Initialize REST baselines with typical resting values
    _baselines[STATE_REST].heartRate.mean = SIM_HR_BASE;
    _baselines[STATE_REST].heartRate.std = 8.0f;
    _baselines[STATE_REST].spo2.mean = SIM_SPO2_BASE;
    _baselines[STATE_REST].spo2.std = 1.2f;
    _baselines[STATE_REST].temperature.mean = SIM_TEMP_BASE;
    _baselines[STATE_REST].temperature.std = 0.3f;

    // Initialize ACTIVE baselines with typical active values
    _baselines[STATE_ACTIVE].heartRate.mean = 110.0f;
    _baselines[STATE_ACTIVE].heartRate.std = 15.0f;
    _baselines[STATE_ACTIVE].spo2.mean = 96.0f;
    _baselines[STATE_ACTIVE].spo2.std = 1.8f;
    _baselines[STATE_ACTIVE].temperature.mean = 37.0f;
    _baselines[STATE_ACTIVE].temperature.std = 0.4f;

    _totalSamples = 0;
}

float BaselineManager::getCurrentAlpha() const {
    if (_totalSamples >= BASELINE_WARMUP_SAMPLES) {
        return BASELINE_ALPHA_END;
    }
    // Linear decay from α_start to α_end during warm-up
    float progress = (float)_totalSamples / (float)BASELINE_WARMUP_SAMPLES;
    return BASELINE_ALPHA_START - (BASELINE_ALPHA_START - BASELINE_ALPHA_END) * progress;
}

BaselinePhase BaselineManager::getPhase() const {
    return (_totalSamples >= BASELINE_WARMUP_SAMPLES) ? PHASE_STABLE : PHASE_WARMUP;
}

float BaselineManager::update(ActivityState state, float hr, float spo2, float temp) {
    float alpha = getCurrentAlpha();

    updateMetric(_baselines[state].heartRate, hr, alpha);
    updateMetric(_baselines[state].spo2, spo2, alpha);
    updateMetric(_baselines[state].temperature, temp, alpha);

    _totalSamples++;
    return alpha;
}

void BaselineManager::updateMetric(MetricBaseline& metric, float value, float alpha) {
    if (metric.sampleCount == 0) {
        // First sample — initialize directly
        metric.mean = value;
        metric.std = 1.0f; // Assume some initial std
    } else {
        // EMA update for mean
        metric.mean = alpha * value + (1.0f - alpha) * metric.mean;
        // EMA update for std (using absolute deviation)
        float deviation = fabsf(value - metric.mean);
        metric.std = alpha * deviation + (1.0f - alpha) * metric.std;
        // Clamp std to a minimum to avoid division by zero in Z-score
        if (metric.std < 0.1f) {
            metric.std = 0.1f;
        }
    }
    metric.sampleCount++;
}

const StateBaseline& BaselineManager::getBaseline(ActivityState state) const {
    return _baselines[state];
}

int BaselineManager::getTotalSamples() const {
    return _totalSamples;
}

ActivityState BaselineManager::classifyActivity(float accelMagnitude) {
    return (accelMagnitude >= ACTIVITY_THRESHOLD) ? STATE_ACTIVE : STATE_REST;
}
