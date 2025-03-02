#include <arduino.h>
#include <logging.h>
#include <flasher.h>

Flasher flasher;

void setup() {
  Log::begin();

  flasher.begin(1000);
}

void loop() {
  Log::println("hello world");

  flasher.flash();

  delay(1000);
}
