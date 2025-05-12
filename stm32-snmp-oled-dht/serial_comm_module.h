#ifndef SERIAL_COMM_MODULE_H
#define SERIAL_COMM_MODULE_H

#include <Arduino.h>

// === CONFIGURABLE DEFINITIONS ===
#define ESP_POWER_PIN 3
#define ESP_SERIAL    Serial
#define ESP_READY_TIMEOUT_MS   4000
#define ESP_RESPONSE_TIMEOUT_MS 4000

struct EspTrapResult {
  bool success;
  String ip;
};

// === Initializes communication pins ===
void initSerialComm() {
  pinMode(ESP_POWER_PIN, OUTPUT);
  digitalWrite(ESP_POWER_PIN, LOW);
  ESP_SERIAL.begin(9600);
}

// === Sends data to ESP and waits for response ===
EspTrapResult sendTrapToESP(float temp, float hum) {
  EspTrapResult result = {false, "Not Set"};

  // Power ON ESP
  digitalWrite(ESP_POWER_PIN, HIGH);
  delay(50); // Brief settle time

  // Wait for "ESP_READY"
  unsigned long readyDeadline = millis() + ESP_READY_TIMEOUT_MS;
  bool espReady = false;

  while (millis() < readyDeadline) {
    if (ESP_SERIAL.available()) {
      String line = ESP_SERIAL.readStringUntil('\n');
      line.trim();
      if (line == "ESP_READY") {
        espReady = true;
        break;
      }
    }
  }

  if (!espReady) {
    digitalWrite(ESP_POWER_PIN, LOW);
    return result; // ESP not ready
  }

  // Send formatted data
  ESP_SERIAL.print("T:");
  ESP_SERIAL.print(temp, 1);
  ESP_SERIAL.print(";H:");
  ESP_SERIAL.println(hum, 1);

  // Wait for ESP_IP response
  unsigned long responseDeadline = millis() + ESP_RESPONSE_TIMEOUT_MS;
  while (millis() < responseDeadline) {
    if (ESP_SERIAL.available()) {
      String response = ESP_SERIAL.readStringUntil('\n');
      response.trim();

      if (response.startsWith("ESP_IP:")) {
        result.ip = response.substring(7);
        result.ip.trim();
        result.success = true;
        break;
      }
    }
  }

  // Power OFF ESP
  digitalWrite(ESP_POWER_PIN, LOW);

  return result;
}

#endif // SERIAL_COMM_MODULE_H

