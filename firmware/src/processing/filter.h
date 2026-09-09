#ifndef FILTER_H
#define FILTER_H

#include <Arduino.h>

/**
 * Moving Average Filter
 * Smooths noisy sensor readings by averaging the last N samples.
 * Template parameter MAX_WINDOW allows compile-time allocation.
 */
template <int MAX_WINDOW = 20>
class MovingAverageFilter {
public:
    MovingAverageFilter() : _windowSize(MAX_WINDOW), _count(0), _index(0), _sum(0.0f) {
        memset(_buffer, 0, sizeof(_buffer));
    }

    void begin(int windowSize) {
        _windowSize = min(windowSize, MAX_WINDOW);
        _count = 0;
        _index = 0;
        _sum = 0.0f;
        memset(_buffer, 0, sizeof(_buffer));
    }

    /**
     * Add a new sample and return the filtered (averaged) value.
     */
    float addSample(float value) {
        // Subtract the oldest value from sum
        _sum -= _buffer[_index];
        // Add new value
        _buffer[_index] = value;
        _sum += value;
        // Advance circular index
        _index = (_index + 1) % _windowSize;
        // Track how many samples we've received (up to windowSize)
        if (_count < _windowSize) {
            _count++;
        }
        return getAverage();
    }

    float getAverage() const {
        if (_count == 0) return 0.0f;
        return _sum / (float)_count;
    }

    bool isFull() const {
        return _count >= _windowSize;
    }

    int getCount() const {
        return _count;
    }

private:
    float _buffer[MAX_WINDOW];
    int   _windowSize;
    int   _count;
    int   _index;
    float _sum;
};

#endif // FILTER_H
