#ifndef MOCK_SERVO_H
#define MOCK_SERVO_H

#include <stdint.h>

/*
 * Mock Servo for testing Joint class.
 * Tracks positions without requiring hardware.
 */
class MockServo {
  private:
    uint16_t _position;
    uint16_t _moveCallCount;
    uint16_t _lastMovedPosition;

  public:
    MockServo() : _position(375), _moveCallCount(0), _lastMovedPosition(0) {}

    void begin() {
      // No-op for mock
    }

    void move(uint16_t position) {
      _position = position;
      _lastMovedPosition = position;
      _moveCallCount++;
    }

    uint16_t getPosition() {
      return _position;
    }

    // Test helper methods
    uint16_t getMoveCallCount() const {
      return _moveCallCount;
    }

    uint16_t getLastMovedPosition() const {
      return _lastMovedPosition;
    }

    void resetCallCount() {
      _moveCallCount = 0;
    }
};

#endif
