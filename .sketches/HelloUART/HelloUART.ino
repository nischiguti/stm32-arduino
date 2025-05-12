void setup() {
  Serial.begin(9600);
  Serial.println("STM32 UART ready");
}

void loop() {
  Serial.println("Hello from STM32 via UART!");
  delay(1000);
}
