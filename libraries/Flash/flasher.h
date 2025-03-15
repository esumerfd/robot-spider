#ifndef FLASHER_H
#define FLASHER_H

#include <arduino.h>

#define ESP_LED_PIN 33

class Flasher {
  private:
    bool _ledOn = false;
    int _ledTransitions = -1;

    void toggleLED();

  public:
    void begin(int flashes = -1);
    void flash();
};

#endif
