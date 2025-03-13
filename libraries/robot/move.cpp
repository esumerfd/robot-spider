#include <move.h>
#include <board.h>

void Move::move() {

  if (directionUp && position <= SERVOMAX) {
    position += DISTANCE;
  }
  else if (!directionUp && position >= SERVOMIN) {
    position -= DISTANCE;
  }
  else {
    directionUp = !directionUp;
  }
}
