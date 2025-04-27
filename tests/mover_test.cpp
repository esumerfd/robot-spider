#include <logging.h>
#include <unit_test.h>
#include <mover_up.h>

void test_mover() {
  MoverUp mover(1, 100);

  int p = mover.move(3);
  SHOULD(p == 8);
  SHOULD(mover.isDirectionUp());

  p = mover.move(8);
  SHOULD(p == 13);
  SHOULD(mover.isDirectionUp());

  p = mover.move(95);
  SHOULD(p == 100);
  SHOULD(mover.isDirectionUp());

  p = mover.move(p);
  SHOULD(p == 100);
  SHOULD_NOT(mover.isDirectionUp());

  p = mover.move(6);
  SHOULD(p == 1);
  SHOULD_NOT(mover.isDirectionUp());

  p = mover.move(p);
  SHOULD(p == 1);
  SHOULD(mover.isDirectionUp());
}
