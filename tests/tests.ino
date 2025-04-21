#include <logging.h>
#include <mover_test.h>

void setup(){
  Log::begin();
  delay(5000);

  Log::println("Start Test Suite");
  test_mover();
  Log::println("Finished Test Suite");
}

void loop(){
  delay(1000);
}
