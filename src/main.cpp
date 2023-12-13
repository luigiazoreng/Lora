#include "Arduino.h"
#include "LoRa_E22.h"
#include "Lidar.h"
#include "Motor.h"

#define ENABLE_RSSI true
#define FREQUENCY_915
#define RX1 27
#define TX1 13

Lidar lidar;
Motor motor;
LoRa_E22 e22ttl(&Serial2, 18, 21, 19); // AUX M0 M1

unsigned long lastSerial1ReadTime = 0;
const unsigned long serial1ReadInterval = 1000; // 1 segundo

void setup()
{
  motor.set();
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX1, TX1);
  delay(500);

  e22ttl.begin();

  ResponseStructContainer c;
  c = e22ttl.getConfiguration();
  Configuration configuration = *(Configuration *)c.data;
  Serial.println(c.status.getResponseDescription());
  configuration.CHAN = 0x17;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
  e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

  Serial.println("Hi, I'm going to send a message!");

  ResponseStatus rs = e22ttl.sendMessage("Hello, I'm the transmitter, the robot ");
  Serial.println(rs.getResponseDescription());
}

void loop()
{
  if (e22ttl.available() > 1)
  {
#ifdef ENABLE_RSSI
    ResponseContainer rc = e22ttl.receiveMessageRSSI();
#else
    ResponseContainer rc = e22ttl.receiveMessage();
#endif

    if (rc.status.code != 1)
    {
      Serial.println(rc.status.getResponseDescription());
    }
    else
    {
      Serial.println(rc.data);
      char command = rc.data.charAt(0);
      switch (command)
      {
      case 'W':
        motor.forward();
        break;
      case 'S':
        motor.backward();
        break;
      case 'A':
        motor.left();
        break;
      case 'D':
        motor.right();
        break;
      case 'P':
        motor.stop();
        break;
      }

#ifdef ENABLE_RSSI
      // Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastSerial1ReadTime >= serial1ReadInterval)
  {
    if (Serial1.available() > 1)
    {
      byte rxBuffer[2048] = {};
      int rxLength = 0;
      try
      {
        rxLength = Serial1.read(rxBuffer, 2048);
      }
      catch (const std::exception &e)
      {
        Serial.println("Error reading data");
      }

      String message = lidar.createString(rxBuffer, rxLength);

      e22ttl.sendMessage(message);
    }

    lastSerial1ReadTime = currentMillis;
  }
}
