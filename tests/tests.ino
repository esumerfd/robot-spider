#include <logging.h>
#include <mover_test.h>

void setup(){
  Log::begin();
  delay(5000);

  Log::println("Start Test Suite");
  delay(1000);
  test_mover();
  Log::println("Finished Test Suite");
  delay(1000);
}

void loop(){
}
