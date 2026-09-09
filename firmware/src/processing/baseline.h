#ifndef BASELINE_H
#define BASELINE_H

#include <Arduino.h>
#include "../config.h"

/**
 * Activity state — determines which baseline set to use.
 */
enum ActivityState {
    STATE_REST = 0,
    STATE_ACTIVE = 1,
    STATE_COUNT = 2      // Number of states (for array sizing)
};

/**
 * Baseline phase — warm-up (fast learning) vs stable (slow update).
 */
enum BaselinePhase {
    PHASE_WARMUP = 0,
    PHASE_STABLE = 1
};

/**
 * Per-metric baseline statistics for one activity state.
 */
struct MetricBaseline {
    float mean;
    float std;
    int   sampleCount;

    MetricBaseline() : mean(0.0f), std(1.0f), sampleCount(0) {}
};

/**
 * Complete baseline data for one activity state.
 */
struct StateBaseline {
    MetricBaseline heartRate;
    MetricBaseline spo2;
    MetricBaseline temperature;
};

/**
 * BaselineManager
 *
 * Maintains personal baselines with:
 * - 2-phase learning: warm-up (α=0.15 decaying to 0.01) then stable (α=0.01)
 * - Activity-aware: separate baselines for REST and ACTIVE states
 */
class BaselineManager {
public:
    BaselineManager();

    /**
     * Initialize with default values (can be loaded from EEPROM/server later).
     */
    void begin();

    /**
     * Update baseline for the given activity state with new sensor readings.
     * Returns the current alpha value used.
     */
    float update(ActivityState state, float hr, float spo2, float temp);

    /**
     * Get the current alpha based on total sample count (2-phase decay).
     */
    float getCurrentAlpha() const;

    /**
     * Get the current baseline phase.
     */
    BaselinePhase getPhase() const;

    /**
     * Get baseline data for a given activity state.
     */
    const StateBaseline& getBaseline(ActivityState state) const;

    /**
     * Get total sample count across both states.
     */
    int getTotalSamples() const;

    /**
     * Classify activity state based on accelerometer magnitude.
     */
    static ActivityState classifyActivity(float accelMagnitude);

private:
    StateBaseline _baselines[STATE_COUNT];  // REST and ACTIVE baselines
    int           _totalSamples;

    /**
     * Update a single metric's baseline using EMA.
     */
    void updateMetric(MetricBaseline& metric, float value, float alpha);
};

#endif // BASELINE_H
