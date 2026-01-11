#ifndef PROFILER_H
#define PROFILER_H

#include <Arduino.h>

/**
 * Profiler - Runtime performance monitoring and diagnostics
 *
 * Provides periodic memory diagnostics and performance profiling capabilities.
 * Logging can be enabled/disabled at construction time to minimize overhead
 * in production builds.
 *
 * Usage:
 *   Profiler profiler(true);  // Enable logging
 *   profiler.update(millis()); // Call in loop()
 */
class Profiler {
  public:
    /**
     * Constructor
     *
     * @param enabled Enable or disable profiling output (default: false)
     * @param intervalMs Logging interval in milliseconds (default: 5000)
     */
    Profiler(bool enabled = false, uint32_t intervalMs = 5000);

    /**
     * Update profiler - call regularly in loop()
     *
     * @param currentMs Current time from millis()
     */
    void update(uint32_t currentMs);

    /**
     * Enable or disable profiling output at runtime
     *
     * @param enabled true to enable logging, false to disable
     */
    void setEnabled(bool enabled);

    /**
     * Check if profiling is currently enabled
     *
     * @return true if profiling is enabled
     */
    bool isEnabled() const;

    /**
     * Set the logging interval
     *
     * @param intervalMs Interval in milliseconds between log outputs
     */
    void setInterval(uint32_t intervalMs);

  private:
    bool _enabled;
    uint32_t _intervalMs;
    uint32_t _lastLogMs;

    void logHeapStats();
};

#endif
