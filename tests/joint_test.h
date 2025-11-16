#ifndef JOINT_TEST_H
#define JOINT_TEST_H

#include <unit_test.h>
#include <logging.h>
#include "mock_servo.h"

// Test-compatible Joint class (uses MockServo instead of Servo)
class TestJoint {
  protected:
    MockServo &_servo;
    uint16_t _currentPos;
    uint16_t _targetPos;
    uint16_t _speed; // Units per second

  public:
    TestJoint(MockServo &servo, uint16_t initialPos)
      : _servo(servo), _currentPos(initialPos), _targetPos(initialPos), _speed(100) {
    }

    void update(uint32_t deltaMs) {
      if (_currentPos == _targetPos) {
        return; // Already at target
      }

      // Calculate maximum distance we can move in this time step
      uint32_t maxDelta = (_speed * deltaMs) / 1000;

      if (maxDelta == 0) {
        return; // Time step too small
      }

      // Move towards target
      if (_currentPos < _targetPos) {
        uint16_t distance = _targetPos - _currentPos;
        if (maxDelta >= distance) {
          _currentPos = _targetPos; // Reached target
        } else {
          _currentPos += maxDelta;
        }
      } else {
        uint16_t distance = _currentPos - _targetPos;
        if (maxDelta >= distance) {
          _currentPos = _targetPos; // Reached target
        } else {
          _currentPos -= maxDelta;
        }
      }

      // Apply position to servo
      _servo.move(_currentPos);
    }

    void setTarget(uint16_t targetPos, uint16_t speed) {
      _targetPos = targetPos;
      _speed = speed;
    }

    uint16_t getPosition() const { return _currentPos; }
    uint16_t getTarget() const { return _targetPos; }
    bool atTarget() const { return _currentPos == _targetPos; }
};

// Test suite for Joint class
namespace JointTest {

  void testInitialization() {
    Log::println("\n=== Joint Initialization Tests ===");

    MockServo servo;
    TestJoint joint(servo, 375);

    SHOULD(joint.getPosition() == 375);
    SHOULD(joint.getTarget() == 375);
    SHOULD(joint.atTarget() == true);
  }

  void testNoMovementWhenAtTarget() {
    Log::println("\n=== Joint No Movement When At Target ===");

    MockServo servo;
    TestJoint joint(servo, 375);

    servo.resetCallCount();
    joint.update(100); // 100ms update

    SHOULD(joint.getPosition() == 375);
    SHOULD(servo.getMoveCallCount() == 0); // Should not call move
  }

  void testMovementUpward() {
    Log::println("\n=== Joint Movement Upward ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(600, 100); // Speed: 100 units/sec

    // After 1000ms (1 second), should move 100 units
    joint.update(1000);
    SHOULD(joint.getPosition() == 200);
    SHOULD(joint.atTarget() == false);

    // After another 1000ms, should move another 100 units
    joint.update(1000);
    SHOULD(joint.getPosition() == 300);

    // Continue until target reached
    joint.update(1000);
    SHOULD(joint.getPosition() == 400);

    joint.update(1000);
    SHOULD(joint.getPosition() == 500);

    joint.update(1000);
    SHOULD(joint.getPosition() == 600);
    SHOULD(joint.atTarget() == true);

    // Further updates should not move
    servo.resetCallCount();
    joint.update(1000);
    SHOULD(joint.getPosition() == 600);
    SHOULD(servo.getMoveCallCount() == 0);
  }

  void testMovementDownward() {
    Log::println("\n=== Joint Movement Downward ===");

    MockServo servo;
    TestJoint joint(servo, 600);

    joint.setTarget(100, 100); // Speed: 100 units/sec

    // After 1000ms (1 second), should move 100 units down
    joint.update(1000);
    SHOULD(joint.getPosition() == 500);
    SHOULD(joint.atTarget() == false);

    // After another 1000ms
    joint.update(1000);
    SHOULD(joint.getPosition() == 400);

    // Continue to target
    joint.update(1000);
    SHOULD(joint.getPosition() == 300);

    joint.update(1000);
    SHOULD(joint.getPosition() == 200);

    joint.update(1000);
    SHOULD(joint.getPosition() == 100);
    SHOULD(joint.atTarget() == true);
  }

  void testNoOvershoot() {
    Log::println("\n=== Joint No Overshoot ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(150, 100); // 50 units away, speed 100 units/sec

    // Update with enough time to go past target
    joint.update(1000); // Would move 100 units, but only 50 away

    SHOULD(joint.getPosition() == 150); // Should stop at target
    SHOULD(joint.atTarget() == true);
  }

  void testSmallDeltaMs() {
    Log::println("\n=== Joint Small DeltaMs ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(200, 100); // Speed: 100 units/sec

    servo.resetCallCount();
    joint.update(5); // 5ms - maxDelta = (100 * 5) / 1000 = 0

    SHOULD(joint.getPosition() == 100); // Should not move
    SHOULD(servo.getMoveCallCount() == 0);

    // Now with 10ms - maxDelta = (100 * 10) / 1000 = 1
    joint.update(10);
    SHOULD(joint.getPosition() == 101); // Should move 1 unit
    SHOULD(servo.getMoveCallCount() == 1);
  }

  void testHighSpeed() {
    Log::println("\n=== Joint High Speed ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(600, 1000); // Speed: 1000 units/sec

    // After 500ms, should move 500 units
    joint.update(500);
    SHOULD(joint.getPosition() == 600);
    SHOULD(joint.atTarget() == true);
  }

  void testServoMoveCalled() {
    Log::println("\n=== Joint Servo Move Called ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(200, 100);

    servo.resetCallCount();
    joint.update(1000); // Should move 100 units

    SHOULD(servo.getMoveCallCount() == 1);
    SHOULD(servo.getLastMovedPosition() == 200);
  }

  void testSetTarget() {
    Log::println("\n=== Joint Set Target ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    joint.setTarget(500, 250);

    SHOULD(joint.getTarget() == 500);

    // Speed should be updated - move 250 units in 1 second
    joint.update(1000);
    SHOULD(joint.getPosition() == 350); // 100 + 250
  }

  void testSequentialTargets() {
    Log::println("\n=== Joint Sequential Targets ===");

    MockServo servo;
    TestJoint joint(servo, 100);

    // First target
    joint.setTarget(300, 200);
    joint.update(1000); // Move 200 units
    SHOULD(joint.getPosition() == 300);
    SHOULD(joint.atTarget() == true);

    // Second target
    joint.setTarget(150, 100);
    joint.update(1000); // Move 100 units down
    SHOULD(joint.getPosition() == 200);
    SHOULD(joint.atTarget() == false);

    joint.update(500); // Move 50 more units
    SHOULD(joint.getPosition() == 150);
    SHOULD(joint.atTarget() == true);
  }

  void runAll() {
    Log::println("\n\n========================================");
    Log::println("       JOINT CLASS TEST SUITE");
    Log::println("========================================");

    testInitialization();
    testNoMovementWhenAtTarget();
    testMovementUpward();
    testMovementDownward();
    testNoOvershoot();
    testSmallDeltaMs();
    testHighSpeed();
    testServoMoveCalled();
    testSetTarget();
    testSequentialTargets();

    Log::println("\n========================================");
    Log::println("       TESTS COMPLETE");
    Log::println("========================================\n");
  }

} // namespace JointTest

#endif
