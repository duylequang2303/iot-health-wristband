#include "classifier.h"
#include <math.h>

// ──────────────────────────────────────────────
// Condition adjustment table
// Each condition's per-metric multipliers
// ──────────────────────────────────────────────
static const ConditionMultiplier CONDITION_TABLE[] = {
    // COND_CAO_HUYET_AP (0x01)
    // Người cao huyết áp – nhịp tim nền đã cao, nhạy hơn với tăng thêm
    { .hrWarning = 0.85f, .hrCritical = 0.85f,
      .spo2Warning = 1.0f, .spo2Critical = 1.0f,
      .tempWarning = 1.0f, .tempCritical = 1.0f,
      .forceRestOnly = false },

    // COND_HEN_SUYEN (0x02)
    // Hen suyễn – SpO2 có thể dao động nhiều hơn, nhạy hơn với SpO2 thấp
    { .hrWarning = 1.0f, .hrCritical = 1.0f,
      .spo2Warning = 0.80f, .spo2Critical = 0.80f,
      .tempWarning = 1.0f, .tempCritical = 1.0f,
      .forceRestOnly = false },

    // COND_TIEU_DUONG (0x04)
    // Tiểu đường – ảnh hưởng tuần hoàn, theo dõi nhịp tim + SpO2 chặt hơn
    { .hrWarning = 0.90f, .hrCritical = 0.90f,
      .spo2Warning = 0.90f, .spo2Critical = 0.90f,
      .tempWarning = 1.0f, .tempCritical = 1.0f,
      .forceRestOnly = false },

    // COND_BENH_TIM (0x08)
    // Bệnh tim – rất nhạy với biến động nhịp tim
    { .hrWarning = 0.70f, .hrCritical = 0.75f,
      .spo2Warning = 0.85f, .spo2Critical = 0.85f,
      .tempWarning = 1.0f, .tempCritical = 1.0f,
      .forceRestOnly = false },

    // COND_NGUOI_CAO_TUOI (0x10)
    // Người cao tuổi (>65) – ngưỡng cảnh báo nhạy hơn tổng thể
    { .hrWarning = 0.85f, .hrCritical = 0.85f,
      .spo2Warning = 0.85f, .spo2Critical = 0.85f,
      .tempWarning = 0.90f, .tempCritical = 0.90f,
      .forceRestOnly = false },

    // COND_HAN_CHE_VAN_DONG (0x20)
    // Hạn chế vận động – baseline chỉ dùng REST, bỏ qua ACTIVE state
    { .hrWarning = 1.0f, .hrCritical = 1.0f,
      .spo2Warning = 1.0f, .spo2Critical = 1.0f,
      .tempWarning = 1.0f, .tempCritical = 1.0f,
      .forceRestOnly = true },
};
static const int NUM_CONDITIONS = 6;


Classifier::Classifier() : _conditionMask(COND_NONE) {
    recalculateMultiplier();
}

void Classifier::setConditions(uint8_t conditionMask) {
    _conditionMask = conditionMask;
    recalculateMultiplier();
}

void Classifier::recalculateMultiplier() {
    // Start with neutral (1.0 = no adjustment)
    _combinedMultiplier.hrWarning    = 1.0f;
    _combinedMultiplier.hrCritical   = 1.0f;
    _combinedMultiplier.spo2Warning  = 1.0f;
    _combinedMultiplier.spo2Critical = 1.0f;
    _combinedMultiplier.tempWarning  = 1.0f;
    _combinedMultiplier.tempCritical = 1.0f;
    _combinedMultiplier.forceRestOnly = false;

    // Multiply all active conditions together
    for (int i = 0; i < NUM_CONDITIONS; i++) {
        if (_conditionMask & (1 << i)) {
            _combinedMultiplier.hrWarning    *= CONDITION_TABLE[i].hrWarning;
            _combinedMultiplier.hrCritical   *= CONDITION_TABLE[i].hrCritical;
            _combinedMultiplier.spo2Warning  *= CONDITION_TABLE[i].spo2Warning;
            _combinedMultiplier.spo2Critical *= CONDITION_TABLE[i].spo2Critical;
            _combinedMultiplier.tempWarning  *= CONDITION_TABLE[i].tempWarning;
            _combinedMultiplier.tempCritical *= CONDITION_TABLE[i].tempCritical;
            if (CONDITION_TABLE[i].forceRestOnly) {
                _combinedMultiplier.forceRestOnly = true;
            }
        }
    }

    // Clamp minimum multiplier to 0.5 (don't reduce thresholds by more than 50%)
    auto clamp = [](float& v) { if (v < 0.5f) v = 0.5f; };
    clamp(_combinedMultiplier.hrWarning);
    clamp(_combinedMultiplier.hrCritical);
    clamp(_combinedMultiplier.spo2Warning);
    clamp(_combinedMultiplier.spo2Critical);
    clamp(_combinedMultiplier.tempWarning);
    clamp(_combinedMultiplier.tempCritical);
}

MetricClassification Classifier::classifyMetric(
    float value, const MetricBaseline& baseline,
    float warningMult, float criticalMult
) {
    MetricClassification result;
    result.value = value;
    result.baselineMean = baseline.mean;
    result.baselineStd = baseline.std;

    // Calculate Z-score
    float zScore = fabsf(value - baseline.mean) / baseline.std;
    result.zScore = zScore;

    // Apply adjusted thresholds
    float adjAttention = Z_NORMAL;                    // Always 1.0 (no adjustment for NORMAL→ATTENTION boundary)
    float adjWarning   = Z_ATTENTION * warningMult;   // Adjusted WARNING threshold
    float adjCritical  = Z_WARNING * criticalMult;    // Adjusted CRITICAL threshold

    if (zScore < adjAttention) {
        result.level = LEVEL_NORMAL;
    } else if (zScore < adjWarning) {
        result.level = LEVEL_ATTENTION;
    } else if (zScore < adjCritical) {
        result.level = LEVEL_WARNING;
    } else {
        result.level = LEVEL_CRITICAL;
    }

    return result;
}

AlertLevel Classifier::checkAbsoluteLimits(float hr, float spo2, float temp) {
    // Regardless of baseline, these absolute limits always trigger CRITICAL
    if (hr < HR_ABS_MIN || hr > HR_ABS_MAX) return LEVEL_CRITICAL;
    if (spo2 < SPO2_ABS_MIN)                return LEVEL_CRITICAL;
    if (temp < TEMP_ABS_MIN || temp > TEMP_ABS_MAX) return LEVEL_CRITICAL;
    return LEVEL_NORMAL;
}

ClassificationResult Classifier::classify(
    float hr, float spo2, float temp,
    ActivityState state, BaselinePhase phase,
    const StateBaseline& baseline
) {
    ClassificationResult result;
    result.activityState = state;
    result.baselinePhase = phase;

    // Check absolute safety limits first (failsafe)
    AlertLevel absLimit = checkAbsoluteLimits(hr, spo2, temp);

    // Classify each metric against baseline with condition adjustments
    result.heartRate = classifyMetric(
        hr, baseline.heartRate,
        _combinedMultiplier.hrWarning, _combinedMultiplier.hrCritical
    );

    result.spo2 = classifyMetric(
        spo2, baseline.spo2,
        _combinedMultiplier.spo2Warning, _combinedMultiplier.spo2Critical
    );

    result.temperature = classifyMetric(
        temp, baseline.temperature,
        _combinedMultiplier.tempWarning, _combinedMultiplier.tempCritical
    );

    // ── Max-rule: overall = max of all individual metrics ──
    result.overall = (AlertLevel)max(
        (int)result.heartRate.level,
        max((int)result.spo2.level, (int)result.temperature.level)
    );

    // Also consider absolute limits
    if (absLimit > result.overall) {
        result.overall = absLimit;
    }

    // ── Warm-up phase: suppress non-critical alerts ──
    if (phase == PHASE_WARMUP && result.overall < LEVEL_CRITICAL) {
        // During warm-up, only report CRITICAL alerts
        // Downgrade everything else to NORMAL to avoid false positives
        result.overall = LEVEL_NORMAL;
        if (result.heartRate.level < LEVEL_CRITICAL) result.heartRate.level = LEVEL_NORMAL;
        if (result.spo2.level < LEVEL_CRITICAL)      result.spo2.level = LEVEL_NORMAL;
        if (result.temperature.level < LEVEL_CRITICAL) result.temperature.level = LEVEL_NORMAL;
    }

    return result;
}

const char* Classifier::levelToString(AlertLevel level) {
    switch (level) {
        case LEVEL_NORMAL:    return "normal";
        case LEVEL_ATTENTION: return "attention";
        case LEVEL_WARNING:   return "warning";
        case LEVEL_CRITICAL:  return "critical";
        default:              return "unknown";
    }
}

const char* Classifier::stateToString(ActivityState state) {
    switch (state) {
        case STATE_REST:   return "rest";
        case STATE_ACTIVE: return "active";
        default:           return "unknown";
    }
}

const char* Classifier::phaseToString(BaselinePhase phase) {
    switch (phase) {
        case PHASE_WARMUP: return "warmup";
        case PHASE_STABLE: return "stable";
        default:           return "unknown";
    }
}
