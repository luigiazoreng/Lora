#include <Arduino.h>

#define UART_RX_PIN 3 // GPIO3
#define UART_TX_PIN  4 // GPIO4

void setup() {
  // Serial connects to the computer
  Serial.begin(115200);
  // Serial2 is the hardware UART port that connects to external circuitry
  Serial2.begin(115200, SERIAL_8N1,
                UART_RX_PIN,
                UART_TX_PIN);
}

void loop() {
  // Copy byte incoming via PC serial
  while (Serial.available() > 0) {
    Serial2.write(Serial.read());
    
  }

  // Copy bytes incoming via UART serial
  while (Serial2.available() > 0) {
    Serial.write(Serial2.read());
  }
}
