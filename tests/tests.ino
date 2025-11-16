#include <logging.h>

void setup(){
  Log::begin();
  delay(5000);

  Log::println("Test Suite - No tests defined yet");
  Log::println("TODO: Add tests for new Joint/Leg architecture");
}

void loop(){
  delay(1000);
}
