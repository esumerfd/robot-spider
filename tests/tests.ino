#include <logging.h>
#include "joint_test.h"

void setup(){
  Log::begin();
  delay(5000);

  // Run Joint class tests
  JointTest::runAll();

  Log::println("\nAll test suites complete!");
}

void loop(){
  delay(1000);
}
