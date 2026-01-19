#include <body.h>
#include <logging.h>
#include <Arduino.h>

Body::Body(Board& board)
  : _board(board),
    // Initialize all 12 servos with their servo numbers (0-11)
    _leftFrontShoulder(board, 0),
    _leftFrontKnee(board, 1),
    _leftMiddleShoulder(board, 2),
    _leftMiddleKnee(board, 3),
    _leftRearShoulder(board, 4),
    _leftRearKnee(board, 5),
    _rightFrontShoulder(board, 6),
    _rightFrontKnee(board, 7),
    _rightMiddleShoulder(board, 8),
    _rightMiddleKnee(board, 9),
    _rightRearShoulder(board, 10),
    _rightRearKnee(board, 11),
    // Initialize all 6 legs with their servos and initial positions (90 degrees)
    _leftFront(_leftFrontShoulder, _leftFrontKnee, 90.0f, 90.0f),
    _leftMiddle(_leftMiddleShoulder, _leftMiddleKnee, 90.0f, 90.0f),
    _leftRear(_leftRearShoulder, _leftRearKnee, 90.0f, 90.0f),
    _rightFront(_rightFrontShoulder, _rightFrontKnee, 90.0f, 90.0f),
    _rightMiddle(_rightMiddleShoulder, _rightMiddleKnee, 90.0f, 90.0f),
    _rightRear(_rightRearShoulder, _rightRearKnee, 90.0f, 90.0f) {

  // Build servo array for iteration
  _servos[0] = &_leftFrontShoulder;
  _servos[1] = &_leftFrontKnee;
  _servos[2] = &_leftMiddleShoulder;
  _servos[3] = &_leftMiddleKnee;
  _servos[4] = &_leftRearShoulder;
  _servos[5] = &_leftRearKnee;
  _servos[6] = &_rightFrontShoulder;
  _servos[7] = &_rightFrontKnee;
  _servos[8] = &_rightMiddleShoulder;
  _servos[9] = &_rightMiddleKnee;
  _servos[10] = &_rightRearShoulder;
  _servos[11] = &_rightRearKnee;

  // Build leg array for iteration
  _legs[0] = &_leftFront;
  _legs[1] = &_leftMiddle;
  _legs[2] = &_leftRear;
  _legs[3] = &_rightFront;
  _legs[4] = &_rightMiddle;
  _legs[5] = &_rightRear;
}

void Body::begin() {
  // Initialize PWM driver once for all servos
  Servo::initializePWM(_board);

  // Initialize all servos (set their initial positions)
  for (int i = 0; i < SERVO_COUNT; i++) {
    _servos[i]->begin();
  }

  Log::println("Body: initialized %d legs with %d servos", LEG_COUNT, SERVO_COUNT);
}

void Body::update(uint32_t deltaMs) {
  // Update all legs based on elapsed time
  for (int i = 0; i < LEG_COUNT; i++) {
    _legs[i]->update(deltaMs);
  }
}

void Body::applyGait(GaitSequence& gait) {
  // Apply sequence to each leg (stateless - can be reapplied)
  // Note: Must call type-specific methods for compile-time type safety
  gait.applyTo(_leftFront);
  gait.applyTo(_leftMiddle);
  gait.applyTo(_leftRear);
  gait.applyTo(_rightFront);
  gait.applyTo(_rightMiddle);
  gait.applyTo(_rightRear);

  Log::println("Gait '%s' applied", gait.getName());
}

bool Body::atTarget() const {
  for (int i = 0; i < LEG_COUNT; i++) {
    if (!_legs[i]->atTarget()) {
      return false;
    }
  }
  return true;
}

void Body::resetToMiddle() {
  float middle = _board.servoMiddle();  // Returns 90.0f
  float speed = _board.servoSpeed();    // Returns 180.0f degrees/sec

  // Set all legs to middle position
  for (int i = 0; i < LEG_COUNT; i++) {
    _legs[i]->shoulder().setTarget(middle, speed);
    _legs[i]->knee().setTarget(middle, speed);
  }

  Log::println("Body: reset to middle position (90°)");
}

bool Body::wiggleServo(const String& servoName) {
  // Map servo name to servo pointer
  Servo* servo = nullptr;

  if (servoName == "leftfrontshoulder") servo = &_leftFrontShoulder;
  else if (servoName == "leftfrontknee") servo = &_leftFrontKnee;
  else if (servoName == "leftmiddleshoulder") servo = &_leftMiddleShoulder;
  else if (servoName == "leftmiddleknee") servo = &_leftMiddleKnee;
  else if (servoName == "leftrearshoulder") servo = &_leftRearShoulder;
  else if (servoName == "leftrearknee") servo = &_leftRearKnee;
  else if (servoName == "rightfrontshoulder") servo = &_rightFrontShoulder;
  else if (servoName == "rightfrontknee") servo = &_rightFrontKnee;
  else if (servoName == "rightmiddleshoulder") servo = &_rightMiddleShoulder;
  else if (servoName == "rightmiddleknee") servo = &_rightMiddleKnee;
  else if (servoName == "rightrearshoulder") servo = &_rightRearShoulder;
  else if (servoName == "rightrearknee") servo = &_rightRearKnee;

  if (servo == nullptr) {
    Log::println("Body: Unknown servo name '%s'", servoName.c_str());
    return false;
  }

  Log::println("Body: Wiggling servo '%s'", servoName.c_str());

  // Wiggle sequence: reset, +10%, -20%, reset
  // 10% of 180° = 18°, 20% = 36°
  const float middle = 90.0f;
  const float plusTenPercent = middle + 18.0f;   // 108°
  const float minusTenPercent = middle - 18.0f;  // 72°
  const int delayMs = 300;

  servo->move(middle);           // Reset to middle
  delay(delayMs);
  servo->move(plusTenPercent);   // +10% (clockwise)
  delay(delayMs);
  servo->move(minusTenPercent);  // -20% from middle (counter-clockwise)
  delay(delayMs);
  servo->move(middle);           // Reset to middle
  delay(delayMs);

  Log::println("Body: Wiggle complete for '%s'", servoName.c_str());
  return true;
}

