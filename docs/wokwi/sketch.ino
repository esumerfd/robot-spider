/**
 * ESP32-CAM Test Sketch - Robot Spider Configuration
 *
 * Tests I2C communication with PCA9685 PWM driver
 * Demonstrates servo control for hexapod robot
 *
 * Pin Configuration:
 * - GPIO14: I2C SCL
 * - GPIO15: I2C SDA
 * - GPIO4: Flash LED / Status LED
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// I2C Configuration for ESP32-CAM
#define SDA_PIN 15
#define SCL_PIN 14
#define LED_PIN 4

// PCA9685 Configuration
#define PCA9685_ADDRESS 0x40
#define SERVO_FREQ 50  // Standard servo frequency (50 Hz)

// Servo pulse width configuration (in microseconds)
#define SERVO_MIN 1000   // Min pulse width (1ms)
#define SERVO_MAX 2000   // Max pulse width (2ms)
#define SERVO_MID 1500   // Center position (1.5ms)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

// Convert microseconds to PCA9685 pulse value
uint16_t microsToPulse(uint16_t micros) {
  // PCA9685 has 12-bit resolution (4096 steps) at 50 Hz
  // 20ms period = 20000 microseconds
  return (uint16_t)((micros * 4096) / 20000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32-CAM Test - Robot Spider Configuration");
  Serial.println("============================================");

  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.printf("I2C initialized on SDA=GPIO%d, SCL=GPIO%d\n", SDA_PIN, SCL_PIN);

  // Scan I2C bus
  Serial.println("\nScanning I2C bus...");
  scanI2C();

  // Initialize PCA9685
  Serial.println("\nInitializing PCA9685...");
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Setup complete!");
  Serial.println("Starting servo test sequence...\n");

  // Blink LED to indicate ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  static unsigned long lastSweepTime = 0;
  static int sweepPosition = 0;
  static int sweepDirection = 1;

  unsigned long currentTime = millis();

  // Sweep servos back and forth every 20ms
  if (currentTime - lastSweepTime >= 20) {
    lastSweepTime = currentTime;

    // Calculate pulse width (sweep between min and max)
    int pulseRange = SERVO_MAX - SERVO_MIN;
    int pulseWidth = SERVO_MIN + (pulseRange * sweepPosition / 100);
    uint16_t pulse = microsToPulse(pulseWidth);

    // Set all servos to the same position
    for (int i = 0; i < 4; i++) {
      pwm.setPWM(i, 0, pulse);
    }

    // Update sweep position
    sweepPosition += sweepDirection;
    if (sweepPosition >= 100) {
      sweepDirection = -1;
      digitalWrite(LED_PIN, HIGH);
    } else if (sweepPosition <= 0) {
      sweepDirection = 1;
      digitalWrite(LED_PIN, LOW);
    }

    // Print status every second
    static unsigned long lastPrintTime = 0;
    if (currentTime - lastPrintTime >= 1000) {
      lastPrintTime = currentTime;
      Serial.printf("Servo position: %d%% (%d us)\n", sweepPosition, pulseWidth);

      // Print heap status
      Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    }
  }
}

void scanI2C() {
  byte error, address;
  int deviceCount = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("  I2C device found at address 0x%02X\n", address);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("  No I2C devices found");
  } else {
    Serial.printf("  Found %d I2C device(s)\n", deviceCount);
  }
}
