#include <robot.h>
#include <logging.h>
#include <arduino.h>

// Constructor with member initializer list (guarantees correct order)
Robot::Robot()
  : _flasher(),
    _board(),
    _body(_board),
    _arcTest(),
    _lastUpdateMs(0),
    _firstLoop(true) {
}

void Robot::setup() {
  Log::begin();
  _flasher.begin();

  delay(500);
  yield(); // Yield to watchdog

  _body.begin();
  yield(); // Yield to watchdog

  _lastUpdateMs = millis();

  Log::println("Robot: setup complete");
  delay(100); // Give serial time to flush

  // Apply initial test sequence to start movement
  // (Moved to first loop iteration to avoid setup crashes)
  // _body.applyGait(_arcTest);
}

void Robot::loop() {
  // Yield to watchdog to prevent ESP32 reset
  yield();

  // Apply gait on first loop iteration
  if (_firstLoop) {
    _body.applyGait(_arcTest);
    _firstLoop = false;
  }

  // Calculate elapsed time since last update
  uint32_t currentMs = millis();
  uint32_t deltaMs = currentMs - _lastUpdateMs;
  _lastUpdateMs = currentMs;

  _flasher.flash(currentMs);

  // Limit deltaMs on first loop to prevent huge jumps
  if (deltaMs > 100) {
    deltaMs = 100; // Cap at 100ms to prevent issues
  }

  // Update all legs (time-based movement)
  _body.update(deltaMs);

  // Check if left front shoulder has reached its target
  // If so, reapply sequence to continue movement (stateless)
  if (_body.leftFront().shoulder().atTarget()) {
    _body.applyGait(_arcTest);
  }

  delay(10); // Small delay for stability, actual timing handled by deltaMs
}
