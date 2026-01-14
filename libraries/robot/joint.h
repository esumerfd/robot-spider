#ifndef JOINT_H
#define JOINT_H

#include <stdint.h>
#include <servo.h>
#include <profiler.h>

/*
 * Base class for joints (Shoulder, Knee).
 *
 * Provides time-based movement with speed control.
 * Joints move smoothly from current position to target position
 * over time based on configured speed.
 */
class Joint {
  protected:
    Servo &_servo;
    float _currentPos;   // Current angle in degrees
    float _targetPos;    // Target angle in degrees
    float _speed;        // Degrees per second

    // Rate limiting for servo writes
    CallRateProfiler _servoWriteProfiler;

  public:
    Joint(Servo &servo, float initialPos);

    // Update joint position based on elapsed time
    virtual void update(uint32_t deltaMs);

    // Set target position and speed
    void setTarget(float targetPos, float speed);

    // Get current position
    float getPosition() const { return _currentPos; }

    // Get target position
    float getTarget() const { return _targetPos; }

    // Check if joint has reached target
    bool atTarget() const { return abs(_currentPos - _targetPos) < 0.5f; }

    // Profiling control
    void enableServoWriteProfiling(bool enabled);
    void setServoWriteRateLimit(uint32_t minIntervalMs);
    CallRateProfiler& getServoWriteProfiler();
};

#endif
