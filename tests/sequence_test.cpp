#include <logging.h>
#include <unit_test.h>
#include <board.h>
#include <servo.h>
#include <sequence.h>

void test_sequence() {
  Board board;
  Servo servo = Servo(board, 0);
  Sequence sequence(servo);

  MoverUp moverUp(1, 100);
  MoverUp moverMoreUp(1, 2);

  sequence.add(moverUp);
  SHOULD(sequence.size() == 1);

  sequence.add(moverMoreUp);
  SHOULD(sequence.size() == 2);

}

