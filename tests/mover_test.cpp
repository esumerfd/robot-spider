#include <logging.h>
#include <mover.h>

void test_mover() {
  Mover mover(1, 100);
  int p = mover.move(3);
  Log::println("PASS: p == 8: %d: but was %d", p == 8, p);
  Log::println("PASS: direction up: %d", mover.isDirectionUp());

  p = mover.move(8);
  Log::println("PASS: p == 13: %d: but was %d", p == 13, p);
  Log::println("PASS: direction up: %d", mover.isDirectionUp());

  p = mover.move(100-5-1);
  Log::println("PASS: p == 99: %d: but was %d", p == 99, p);
  Log::println("PASS: direction up: %d", mover.isDirectionUp());

  p = mover.move(100-1);
  Log::println("PASS: p == 100-1: %d: but was %d", p == 99, p);
  Log::println("PASS: direction down: %d", mover.isDirectionUp());
}
