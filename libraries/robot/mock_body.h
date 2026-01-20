#ifndef MOCK_BODY_H
#define MOCK_BODY_H

#include <Arduino.h>
#include <i_gait_target.h>
#include <gait_sequence.h>
#include <logging.h>

/**
 * Simulated joint for testing - tracks position without hardware.
 */
class MockJoint {
  private:
    const char* _name;
    float _currentPos;
    float _targetPos;
    float _speed;

  public:
    MockJoint(const char* name, float initialPos = 90.0f)
      : _name(name),
        _currentPos(initialPos),
        _targetPos(initialPos),
        _speed(180.0f) {}

    void setTarget(float target, float speed) {
      _targetPos = target;
      _speed = speed;
    }

    void update(uint32_t deltaMs) {
      if (atTarget()) return;

      float maxDelta = (_speed * deltaMs) / 1000.0f;
      float direction = (_currentPos < _targetPos) ? 1.0f : -1.0f;
      float distance = abs(_targetPos - _currentPos);

      if (maxDelta >= distance) {
        _currentPos = _targetPos;
      } else {
        _currentPos += direction * maxDelta;
      }
    }

    bool atTarget() const {
      return abs(_currentPos - _targetPos) < 0.5f;
    }

    float getPosition() const { return _currentPos; }
    float getTarget() const { return _targetPos; }
    const char* getName() const { return _name; }

    void reset(float pos = 90.0f) {
      _currentPos = pos;
      _targetPos = pos;
    }
};

/**
 * Simulated leg for testing - contains shoulder and knee joints.
 */
class MockLeg {
  private:
    MockJoint _shoulder;
    MockJoint _knee;

  public:
    MockLeg(const char* shoulderName, const char* kneeName)
      : _shoulder(shoulderName),
        _knee(kneeName) {}

    MockJoint& shoulder() { return _shoulder; }
    MockJoint& knee() { return _knee; }
    const MockJoint& shoulder() const { return _shoulder; }
    const MockJoint& knee() const { return _knee; }

    void update(uint32_t deltaMs) {
      _shoulder.update(deltaMs);
      _knee.update(deltaMs);
    }

    bool atTarget() const {
      return _shoulder.atTarget() && _knee.atTarget();
    }

    void reset() {
      _shoulder.reset();
      _knee.reset();
    }
};

/**
 * Mock body for testing gait sequences without hardware.
 *
 * Simulates 6 legs with 2 joints each, tracking positions in software.
 * No servo writes occur - just state tracking and logging.
 */
class MockBody : public IGaitTarget {
  private:
    MockLeg _leftFront;
    MockLeg _leftMiddle;
    MockLeg _leftRear;
    MockLeg _rightFront;
    MockLeg _rightMiddle;
    MockLeg _rightRear;

    // Default speed for movements (degrees per second)
    static constexpr float DEFAULT_SPEED = 180.0f;

    // Safe angle limits
    static constexpr float SAFE_MIN = 2.0f;
    static constexpr float SAFE_MAX = 178.0f;

    float clamp(float value, float minVal, float maxVal) const {
      if (value < minVal) return minVal;
      if (value > maxVal) return maxVal;
      return value;
    }

  public:
    MockBody()
      : _leftFront("LF.shoulder", "LF.knee"),
        _leftMiddle("LM.shoulder", "LM.knee"),
        _leftRear("LR.shoulder", "LR.knee"),
        _rightFront("RF.shoulder", "RF.knee"),
        _rightMiddle("RM.shoulder", "RM.knee"),
        _rightRear("RR.shoulder", "RR.knee") {}

    // Note: applyGait cannot be used with GaitSequence directly because
    // GaitSequence uses specific leg types. Use applyStep() instead.
    void applyGait(GaitSequence& gait) override {
      Log::println("MockBody: applyGait() not supported - use TestHarness.applyStep()");
    }

    // Direct methods for applying movements (called by gait sequences)
    void applyLeftFront(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_leftFront.shoulder(), shoulderDelta);
      applyDelta(_leftFront.knee(), kneeDelta);
    }

    void applyLeftMiddle(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_leftMiddle.shoulder(), shoulderDelta);
      applyDelta(_leftMiddle.knee(), kneeDelta);
    }

    void applyLeftRear(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_leftRear.shoulder(), shoulderDelta);
      applyDelta(_leftRear.knee(), kneeDelta);
    }

    void applyRightFront(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_rightFront.shoulder(), shoulderDelta);
      applyDelta(_rightFront.knee(), kneeDelta);
    }

    void applyRightMiddle(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_rightMiddle.shoulder(), shoulderDelta);
      applyDelta(_rightMiddle.knee(), kneeDelta);
    }

    void applyRightRear(int8_t shoulderDelta, int8_t kneeDelta) {
      applyDelta(_rightRear.shoulder(), shoulderDelta);
      applyDelta(_rightRear.knee(), kneeDelta);
    }

    void update(uint32_t deltaMs) override {
      _leftFront.update(deltaMs);
      _leftMiddle.update(deltaMs);
      _leftRear.update(deltaMs);
      _rightFront.update(deltaMs);
      _rightMiddle.update(deltaMs);
      _rightRear.update(deltaMs);
    }

    bool atTarget() const override {
      return _leftFront.atTarget() &&
             _leftMiddle.atTarget() &&
             _leftRear.atTarget() &&
             _rightFront.atTarget() &&
             _rightMiddle.atTarget() &&
             _rightRear.atTarget();
    }

    void resetToMiddle() override {
      _leftFront.reset();
      _leftMiddle.reset();
      _leftRear.reset();
      _rightFront.reset();
      _rightMiddle.reset();
      _rightRear.reset();
      Log::println("MockBody: Reset to middle (90 degrees)");
    }

    void logState() const override {
      Log::println("MockBody State:");
      logLeg("  LF", _leftFront);
      logLeg("  LM", _leftMiddle);
      logLeg("  LR", _leftRear);
      logLeg("  RF", _rightFront);
      logLeg("  RM", _rightMiddle);
      logLeg("  RR", _rightRear);
    }

  private:
    void applyDelta(MockJoint& joint, int8_t delta) {
      if (delta == 0) return;

      float currentPos = joint.getPosition();
      float newTarget = currentPos + (float)delta;
      newTarget = clamp(newTarget, SAFE_MIN, SAFE_MAX);

      joint.setTarget(newTarget, DEFAULT_SPEED);
    }

    void logLeg(const char* prefix, const MockLeg& leg) const {
      Log::println("%s: sh=%.1f->%.1f kn=%.1f->%.1f %s",
                   prefix,
                   leg.shoulder().getPosition(),
                   leg.shoulder().getTarget(),
                   leg.knee().getPosition(),
                   leg.knee().getTarget(),
                   leg.atTarget() ? "[at target]" : "[moving]");
    }
};

#endif
