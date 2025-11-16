#include <body.h>
#include <logging.h>

Body::Body(Board& board)
  : // Initialize all 12 servos with their servo numbers (0-11)
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
}

void Body::begin() {
  // Initialize all servos
  _leftFrontShoulder.begin();
  _leftFrontKnee.begin();
  _leftMiddleShoulder.begin();
  _leftMiddleKnee.begin();
  _leftRearShoulder.begin();
  _leftRearKnee.begin();
  _rightFrontShoulder.begin();
  _rightFrontKnee.begin();
  _rightMiddleShoulder.begin();
  _rightMiddleKnee.begin();
  _rightRearShoulder.begin();
  _rightRearKnee.begin();

  Log::println("Body: initialized 6 legs with 12 servos");
}

void Body::update(uint32_t deltaMs) {
  // Update all legs based on elapsed time
  _leftFront.update(deltaMs);
  _leftMiddle.update(deltaMs);
  _leftRear.update(deltaMs);
  _rightFront.update(deltaMs);
  _rightMiddle.update(deltaMs);
  _rightRear.update(deltaMs);
}

void Body::applyGait(GaitSequence& gait) {
  // Apply sequence to each leg (stateless - can be reapplied)
  gait.applyTo(_leftFront);
  gait.applyTo(_leftMiddle);
  gait.applyTo(_leftRear);
  gait.applyTo(_rightFront);
  gait.applyTo(_rightMiddle);
  gait.applyTo(_rightRear);

  Log::println("Body: applied gait '%s' to all legs", gait.getName());
}

