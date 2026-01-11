#include "profiler.h"
#include <logging.h>

Profiler::Profiler(bool enabled, uint32_t intervalMs)
  : _enabled(enabled),
    _intervalMs(intervalMs),
    _lastLogMs(0) {
}

void Profiler::update(uint32_t currentMs) {
  if (!_enabled) {
    return;
  }

  // Check if it's time to log
  if (currentMs - _lastLogMs >= _intervalMs) {
    logHeapStats();
    _lastLogMs = currentMs;
  }
}

void Profiler::setEnabled(bool enabled) {
  _enabled = enabled;

  if (_enabled) {
    Log::println("Profiler: Enabled");
  } else {
    Log::println("Profiler: Disabled");
  }
}

bool Profiler::isEnabled() const {
  return _enabled;
}

void Profiler::setInterval(uint32_t intervalMs) {
  _intervalMs = intervalMs;
  Log::println("Profiler: Interval set to %d ms", intervalMs);
}

void Profiler::logHeapStats() {
  Log::println("Heap: %d bytes (min: %d)", ESP.getFreeHeap(), ESP.getMinFreeHeap());
}
