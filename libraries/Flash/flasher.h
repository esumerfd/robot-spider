#ifndef FLASHER_H
#define FLASHER_H

#include <arduino.h>

#define ESP_LED_PIN 33

class Flasher {
  private:
    bool _ledOn = false;
    int _ledTransitions = 0;

    void toggleLED() {
      digitalWrite(ESP_LED_PIN, _ledOn ? LOW : HIGH);
      _ledOn = !_ledOn;
    }

  public:
    void begin(int flashers) {
      _ledTransitions = flashers * 2;

      pinMode(ESP_LED_PIN, OUTPUT);
    }

    void flash() {
      if (_ledTransitions > 0) {
        _ledTransitions--;
        toggleLED();
      }
    }
};

#endif
