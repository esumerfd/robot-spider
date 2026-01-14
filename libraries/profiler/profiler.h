#ifndef PROFILER_H
#define PROFILER_H

#include <Arduino.h>

/**
 * BaseProfiler - Abstract base class for profilers
 *
 * Provides common functionality for periodic profiling and diagnostics.
 * Subclasses implement specific profiling behavior in logStats().
 */
class BaseProfiler {
  public:
    /**
     * Constructor
     *
     * @param enabled Enable or disable profiling output (default: false)
     * @param intervalMs Logging interval in milliseconds (default: 5000)
     */
    BaseProfiler(bool enabled = false, uint32_t intervalMs = 5000);

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

  protected:
    bool _enabled;
    uint32_t _intervalMs;
    uint32_t _lastLogMs;

    /**
     * Log profiling statistics - must be implemented by subclasses
     */
    virtual void logStats() = 0;
};

/**
 * MemoryProfiler - Memory usage diagnostics
 *
 * Logs ESP32 heap memory statistics at regular intervals.
 *
 * Usage:
 *   MemoryProfiler profiler(true);  // Enable logging
 *   profiler.update(millis());      // Call in loop()
 */
class MemoryProfiler : public BaseProfiler {
  public:
    /**
     * Constructor
     *
     * @param enabled Enable or disable profiling output (default: false)
     * @param intervalMs Logging interval in milliseconds (default: 5000)
     */
    MemoryProfiler(bool enabled = false, uint32_t intervalMs = 5000);

  protected:
    void logStats() override;
};

/**
 * CallRateProfiler - Call rate tracking and diagnostics
 *
 * Tracks number of calls to tick() and calculates calls per second.
 * Useful for measuring servo update rates, gait application frequency, etc.
 *
 * Can also enforce rate limiting by executing callbacks only when
 * sufficient time has passed since the last execution.
 *
 * Usage:
 *   CallRateProfiler profiler("ServoWrite", true, 1000, 20);  // Min 20ms between executions
 *   profiler.update(millis());                                 // Call in loop()
 *   profiler.executeIfReady(millis(), []() { servo.move(); }); // Rate-limited execution
 */
class CallRateProfiler : public BaseProfiler {
  public:
    /**
     * Constructor
     *
     * @param name Name for this profiler (e.g., "ServoWrite", "GaitApply")
     * @param enabled Enable or disable profiling output (default: false)
     * @param intervalMs Logging interval in milliseconds (default: 1000)
     * @param minIntervalMs Minimum time between executions in ms (0 = no limit, default: 0)
     */
    CallRateProfiler(const char* name, bool enabled = false, uint32_t intervalMs = 1000, uint32_t minIntervalMs = 0);

    /**
     * Record a call event
     * Call this each time the event being tracked occurs
     */
    void tick();

    /**
     * Execute callback only if minimum interval has passed since last execution
     * Tracks both attempted calls and successful executions
     *
     * @param currentMs Current time from millis()
     * @param callback Function to execute if rate limit allows
     * @return true if callback was executed, false if rate limited
     */
    template<typename Callback>
    bool executeIfReady(uint32_t currentMs, Callback callback) {
      _attemptedCalls++;

      // Check if enough time has passed since last execution
      if (_minIntervalMs > 0 && (currentMs - _lastExecuteMs < _minIntervalMs)) {
        return false;  // Rate limited
      }

      // Execute callback
      callback();
      _executedCalls++;
      _lastExecuteMs = currentMs;
      return true;
    }

    /**
     * Get the current call rate in calls per second
     *
     * @return Calls per second (averaged over interval)
     */
    float getCallRate() const;

    /**
     * Get total number of calls since last reset
     *
     * @return Total call count
     */
    uint32_t getTotalCalls() const;

    /**
     * Get total number of attempted executions (including rate limited)
     *
     * @return Total attempted execution count
     */
    uint32_t getAttemptedCalls() const;

    /**
     * Get total number of successful executions (not rate limited)
     *
     * @return Total executed call count
     */
    uint32_t getExecutedCalls() const;

  protected:
    void logStats() override;

  private:
    const char* _name;
    uint32_t _callCount;        // Total tick() calls
    uint32_t _lastCallCount;    // Last logged tick() count
    float _callRate;            // Calculated calls per second

    // Rate limiting
    uint32_t _minIntervalMs;    // Minimum ms between executions (0 = no limit)
    uint32_t _lastExecuteMs;    // Last execution time
    uint32_t _attemptedCalls;   // Total executeIfReady() attempts
    uint32_t _executedCalls;    // Successful executions (not rate limited)
};

#endif
