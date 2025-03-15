#include <arduino.h>
#include <flasher.h>

void Flasher::toggleLED() {
  digitalWrite(ESP_LED_PIN, _ledOn ? LOW : HIGH);
  _ledOn = !_ledOn; 
}

void Flasher::begin(int flashes) {
  if (flashes > 0) _ledTransitions = flashes * 2;

  pinMode(ESP_LED_PIN, OUTPUT);
}

void Flasher::flash() {
  if (_ledTransitions > 0) _ledTransitions--;

  if (_ledTransitions < 0 || _ledTransitions > 0) {
    toggleLED();
  }
}
