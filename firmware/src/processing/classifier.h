#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <Arduino.h>
#include "../config.h"
#include "baseline.h"

/**
 * Alert levels — ordered by severity.
 */
enum AlertLevel {
    LEVEL_NORMAL    = 0,
    LEVEL_ATTENTION = 1,
    LEVEL_WARNING   = 2,
    LEVEL_CRITICAL  = 3
};

/**
 * Per-metric classification result.
 */
struct MetricClassification {
    AlertLevel level;
    float      zScore;
    float      value;
    float      baselineMean;
    float      baselineStd;
};

/**
 * Complete classification result for all metrics + overall.
 */
struct ClassificationResult {
    MetricClassification heartRate;
    MetricClassification spo2;
    MetricClassification temperature;
    AlertLevel           overall;       // Max-rule across all metrics
    ActivityState        activityState;
    BaselinePhase        baselinePhase;
};

/**
 * Condition-based threshold adjustment multipliers.
 * Each condition modifies the Z-score thresholds per metric.
 */
struct ConditionMultiplier {
    float hrWarning;
    float hrCritical;
    float spo2Warning;
    float spo2Critical;
    float tempWarning;
    float tempCritical;
    bool  forceRestOnly;  // If true, ignore ACTIVE state (e.g., mobility-limited)
};

/**
 * Classifier
 *
 * Classifies sensor readings into 4 alert levels using Z-scores
 * against personal baselines. Supports:
 * - Per-metric classification
 * - Max-rule composite (overall = max of all metrics)
 * - Condition-based threshold adjustments
 * - Warm-up phase handling (only CRITICAL alerts during warm-up)
 */
class Classifier {
public:
    Classifier();

    /**
     * Set active health conditions (bitmask).
     * Call when user profile changes.
     */
    void setConditions(uint8_t conditionMask);

    /**
     * Classify all metrics against the appropriate baseline.
     */
    ClassificationResult classify(
        float hr, float spo2, float temp,
        ActivityState state, BaselinePhase phase,
        const StateBaseline& baseline
    );

    /**
     * Get string representation of an alert level.
     */
    static const char* levelToString(AlertLevel level);

    /**
     * Get string representation of activity state.
     */
    static const char* stateToString(ActivityState state);

    /**
     * Get string representation of baseline phase.
     */
    static const char* phaseToString(BaselinePhase phase);

    // ── Condition bitmask constants ──
    static const uint8_t COND_NONE              = 0x00;
    static const uint8_t COND_CAO_HUYET_AP      = 0x01;
    static const uint8_t COND_HEN_SUYEN         = 0x02;
    static const uint8_t COND_TIEU_DUONG        = 0x04;
    static const uint8_t COND_BENH_TIM          = 0x08;
    static const uint8_t COND_NGUOI_CAO_TUOI    = 0x10;
    static const uint8_t COND_HAN_CHE_VAN_DONG  = 0x20;

private:
    uint8_t _conditionMask;

    // Combined multiplier (product of all active conditions)
    ConditionMultiplier _combinedMultiplier;

    /**
     * Recalculate combined multiplier from condition mask.
     */
    void recalculateMultiplier();

    /**
     * Classify a single metric.
     */
    MetricClassification classifyMetric(
        float value, const MetricBaseline& baseline,
        float warningMult, float criticalMult
    );

    /**
     * Check absolute safety limits (failsafe).
     */
    AlertLevel checkAbsoluteLimits(float hr, float spo2, float temp);
};

#endif // CLASSIFIER_H
