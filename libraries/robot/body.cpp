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
    // Initialize all 6 legs with their servos and initial positions
    _leftFront(_leftFrontShoulder, _leftFrontKnee, board.servoMiddle(), board.servoMiddle()),
    _leftMiddle(_leftMiddleShoulder, _leftMiddleKnee, board.servoMiddle(), board.servoMiddle()),
    _leftRear(_leftRearShoulder, _leftRearKnee, board.servoMiddle(), board.servoMiddle()),
    _rightFront(_rightFrontShoulder, _rightFrontKnee, board.servoMiddle(), board.servoMiddle()),
    _rightMiddle(_rightMiddleShoulder, _rightMiddleKnee, board.servoMiddle(), board.servoMiddle()),
    _rightRear(_rightRearShoulder, _rightRearKnee, board.servoMiddle(), board.servoMiddle()) {

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

