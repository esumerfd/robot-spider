#include <stdio.h>
#include <mover.h>
#include <cassert>

void test_mover() {
  printf("Test mover\n");
  Mover mover(1, 6);
  int p = mover.move(3);
  assert(p == 1);
}
