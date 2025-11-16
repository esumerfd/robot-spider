#include <robot.h>
#include <logging.h>
#include <arduino.h>

// Constructor with member initializer list (guarantees correct order)
Robot::Robot()
  : _flasher(),
    _board(),
    _body(_board),
    _arcTest(),
    _lastUpdateMs(0) {
}

void Robot::setup() {
  Log::begin();
  _flasher.begin();

  delay(500);

  _body.begin();

  // Apply initial test sequence to start movement
  _body.applyGait(_arcTest);

  _lastUpdateMs = millis();

  Log::println("Robot: initialized with 6 legs, running ArcTest sequence");
}

void Robot::loop() {
  // Calculate elapsed time since last update
  uint32_t currentMs = millis();
  uint32_t deltaMs = currentMs - _lastUpdateMs;
  _lastUpdateMs = currentMs;

  _flasher.flash(currentMs);

  // Update all legs (time-based movement)
  _body.update(deltaMs);

  // Check if left front shoulder has reached its target
  // If so, reapply sequence to continue movement (stateless)
  if (_body.leftFront().shoulder().atTarget()) {
    _body.applyGait(_arcTest);
  }

  delay(10); // Small delay for stability, actual timing handled by deltaMs
}
