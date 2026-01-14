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
    uint16_t _currentPos;
    uint16_t _targetPos;
    uint16_t _speed; // Units per second

    // Rate limiting for servo writes
    CallRateProfiler _servoWriteProfiler;

  public:
    Joint(Servo &servo, uint16_t initialPos);

    // Update joint position based on elapsed time
    virtual void update(uint32_t deltaMs);

    // Set target position and speed
    void setTarget(uint16_t targetPos, uint16_t speed);

    // Get current position
    uint16_t getPosition() const { return _currentPos; }

    // Get target position
    uint16_t getTarget() const { return _targetPos; }

    // Check if joint has reached target
    bool atTarget() const { return _currentPos == _targetPos; }

    // Profiling control
    void enableServoWriteProfiling(bool enabled);
    void setServoWriteRateLimit(uint32_t minIntervalMs);
    CallRateProfiler& getServoWriteProfiler();
};

#endif
