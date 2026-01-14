#include "profiler.h"
#include <logging.h>

// ============================================================================
// BaseProfiler Implementation
// ============================================================================

BaseProfiler::BaseProfiler(bool enabled, uint32_t intervalMs)
  : _enabled(enabled),
    _intervalMs(intervalMs),
    _lastLogMs(0) {
}

void BaseProfiler::update(uint32_t currentMs) {
  if (!_enabled) {
    return;
  }

  // Check if it's time to log
  if (currentMs - _lastLogMs >= _intervalMs) {
    logStats();
    _lastLogMs = currentMs;
  }
}

void BaseProfiler::setEnabled(bool enabled) {
  _enabled = enabled;

  if (_enabled) {
    Log::println("Profiler: Enabled");
  } else {
    Log::println("Profiler: Disabled");
  }
}

bool BaseProfiler::isEnabled() const {
  return _enabled;
}

void BaseProfiler::setInterval(uint32_t intervalMs) {
  _intervalMs = intervalMs;
  Log::println("Profiler: Interval set to %d ms", intervalMs);
}

// ============================================================================
// MemoryProfiler Implementation
// ============================================================================

MemoryProfiler::MemoryProfiler(bool enabled, uint32_t intervalMs)
  : BaseProfiler(enabled, intervalMs) {
}

void MemoryProfiler::logStats() {
  Log::println("Memory: %d bytes free (min: %d bytes)", ESP.getFreeHeap(), ESP.getMinFreeHeap());
}

// ============================================================================
// CallRateProfiler Implementation
// ============================================================================

CallRateProfiler::CallRateProfiler(const char* name, bool enabled, uint32_t intervalMs, uint32_t minIntervalMs)
  : BaseProfiler(enabled, intervalMs),
    _name(name),
    _callCount(0),
    _lastCallCount(0),
    _callRate(0.0f),
    _minIntervalMs(minIntervalMs),
    _lastExecuteMs(0),
    _attemptedCalls(0),
    _executedCalls(0) {
}

void CallRateProfiler::tick() {
  _callCount++;
}

float CallRateProfiler::getCallRate() const {
  return _callRate;
}

uint32_t CallRateProfiler::getTotalCalls() const {
  return _callCount;
}

uint32_t CallRateProfiler::getAttemptedCalls() const {
  return _attemptedCalls;
}

uint32_t CallRateProfiler::getExecutedCalls() const {
  return _executedCalls;
}

void CallRateProfiler::logStats() {
  // Calculate calls since last log
  uint32_t callsSinceLastLog = _callCount - _lastCallCount;
  _lastCallCount = _callCount;

  // Calculate rate (calls per second)
  // intervalMs is the time window, convert to seconds
  _callRate = (callsSinceLastLog * 1000.0f) / _intervalMs;

  // Log with rate limiting stats if enabled
  if (_minIntervalMs > 0) {
    uint32_t rateLimited = _attemptedCalls - _executedCalls;
    Log::println("%s: %.2f Hz (%.0f calls in %d ms, %d total) | Rate limit: %d ms | Executed: %d/%d (%.1f%%)",
                 _name, _callRate, (float)callsSinceLastLog, _intervalMs, _callCount,
                 _minIntervalMs, _executedCalls, _attemptedCalls,
                 _attemptedCalls > 0 ? (100.0f * _executedCalls / _attemptedCalls) : 0.0f);
  } else {
    Log::println("%s: %.2f Hz (%.0f calls in %d ms, %d total calls)",
                 _name, _callRate, (float)callsSinceLastLog, _intervalMs, _callCount);
  }
}
