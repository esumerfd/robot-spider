#ifndef FLASHER_H
#define FLASHER_H

#include <arduino.h>

#define ESP_LED_PIN 33

class Flasher {
  private:
    bool _ledOn = false;
    int _ledTransitions = -1;
    uint32_t _lastToggleMs = 0;
    uint32_t _toggleIntervalMs = 500; // Flash every 500ms

    void toggleLED();

  public:
    void begin(int flashes = -1);
    void flash(uint32_t currentMs);
};

#endif
