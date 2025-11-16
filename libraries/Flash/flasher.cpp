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

void Flasher::flash(uint32_t currentMs) {
  // Only toggle at specified interval
  if (currentMs - _lastToggleMs < _toggleIntervalMs) {
    return;
  }

  _lastToggleMs = currentMs;

  if (_ledTransitions > 0) _ledTransitions--;

  if (_ledTransitions < 0 || _ledTransitions > 0) {
    toggleLED();
  }
}
