#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include "serial_comm_module.h"  // Custom module if used; else remove

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// STM32F103C8T6 Blue Pill Pin Mapping
#define DHTPIN PA8              // Data pin for DHT22
#define ESP_POWER_PIN PB0       // ESP-01S power control via MOSFET
#define LED_BUILTIN PC13        // Onboard LED (active-low)

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Environmental tracking
float minTemp = 1000.0, maxTemp = -1000.0;
float minHum  = 1000.0, maxHum  = -1000.0;

// Logo positions and content
int baseX = 7, baseY = 3;
String espIP = "Not Set";
bool espConnected = false;
unsigned long espTriggerInterval = 1;  // Minutes
unsigned long lastEspTriggerMillis = 0;

// --------------- Logo Bitmaps ---------------
const unsigned char Wifi_Logo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x3f, 0xfc, 0x7f, 0xfe, 0x70, 0x0e, 0x23, 0xc6, 
  0x0f, 0xf0, 0x1f, 0xf8, 0x04, 0x20, 0x01, 0x80, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM Temperature_Logo[] = {
  B00011000, B00000000, B00100100, B00111111, B00100100, B00000000, B00100100, B00111111,
  B00100100, B00000000, B00100100, B00111111, B00100100, B00000000, B01000010, B00111111,
  B10011001, B00000000, B10111101, B00000000, B10111101, B00000000, B10011001, B00000000,
  B01000010, B00000000, B00111100, B00000000
};

static const unsigned char PROGMEM Humidity_Logo[] = {
  B00001000, B00000000, B00011100, B00000000, B00111110, B00000000, B01111111, B00000000,
  B01111111, B00000000, B00111110, B00000000, B00011100, B00000000, B00000000, B00010000,
  B00000000, B00111000, B00000000, B01111100, B00000000, B11111110, B00000000, B11111110,
  B00000000, B01111100, B00000000, B00111000
};
// -------------------------------------------

void setup() {
  delay(100);
  Serial.begin(9600);
  dht.begin();

  pinMode(DHTPIN, INPUT);
  pinMode(ESP_POWER_PIN, OUTPUT);
  digitalWrite(ESP_POWER_PIN, LOW); // ESP off by default
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // LED off (active-low)

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED not found at 0x3C");
    while (true);  // Halt
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("DHT22 + OLED Ready");
  display.display();
  delay(1000);
}

void loop() {
  static bool led = false;
  digitalWrite(LED_BUILTIN, led ? LOW : HIGH);
  led = !led;

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  unsigned long currentMillis = millis();
  if ((currentMillis - lastEspTriggerMillis) >= espTriggerInterval * 60000UL) {
    Serial.println("⏱️ Triggering ESP-01S for SNMP...");

    espConnected = false;
    espIP = "Not Set";

    digitalWrite(ESP_POWER_PIN, HIGH);
    delay(100);

    Serial.print("T:");
    Serial.print(temp, 1);
    Serial.print(";H:");
    Serial.println(hum, 1);

    unsigned long timeout = millis() + 4000;
    while (millis() < timeout) {
      if (Serial.available()) {
        String response = Serial.readStringUntil('\n');
        response.trim();
        if (response.startsWith("ESP_IP:")) {
          espIP = response.substring(7);
          espConnected = true;
          Serial.println("✅ ESP responded: " + espIP);
          break;
        }
      }
    }

    digitalWrite(ESP_POWER_PIN, LOW);

    if (!espConnected) {
      Serial.println("❌ ESP did not respond in time.");
    }

    if (espConnected) {
      minTemp = min(minTemp, temp);
      maxTemp = max(maxTemp, temp);
      minHum  = min(minHum, hum);
      maxHum  = max(maxHum, hum);
    }

    lastEspTriggerMillis = currentMillis;
  }

  updateDisplay(temp, hum, espIP);
  delay(5000); // Delay for readability; change for real-time use
}

void updateDisplay(float temp, float hum, String ip) {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(baseX + 2, baseY + 1, Temperature_Logo, 16, 14, WHITE);
  display.setCursor(baseX + 31, baseY + 4);
  display.print(!isnan(temp) ? String(temp, 1) : "Err");
  display.setCursor(baseX + 67, baseY + 4);
  display.print((char)247);  // degree symbol
  display.print("C");

  display.drawBitmap(baseX + 2, baseY + 22, Humidity_Logo, 16, 14, WHITE);
  display.setCursor((hum < 100) ? (baseX + 31) : (baseX + 28), baseY + 23);
  display.print(!isnan(hum) ? String(hum, 1) + " %" : "Err");

  display.drawBitmap(baseX + 2, baseY + 40, Wifi_Logo, 16, 16, WHITE);
  char ipStr[17]; ip.toCharArray(ipStr, sizeof(ipStr));
  display.fillRect(baseX + 25, baseY + 44, 80, 10, BLACK);
  display.setCursor(baseX + 25, baseY + 44);
  display.print(ipStr);

  delay(50);
  display.display();
}

