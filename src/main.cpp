#include "Arduino.h"
#include "LoRa_E22.h"
#include "Lidar.h"
#include "Motor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ENABLE_RSSI true
#define FREQUENCY_915
#define RX1 27
#define TX1 13

Lidar lidar;
Motor motor;
LoRa_E22 e22ttl(&Serial2, 18, 21, 19); // AUX M0 M1

unsigned long lastSerial1ReadTime = 0;
const unsigned long serial1ReadInterval = 1500; // 2 segundos


void receiveCommandTask(void *parameter)
{
  while (1)
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
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1); // Ensure fairness to other tasks
  }
}

void sendLidarDataTask(void *parameter)
{
  while (1)
  {
    if (Serial1.available() > 0)
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


      vTaskDelay(10 / portTICK_PERIOD_MS); // Adjust delay as needed
    }
    vTaskDelay(1); // Ensure fairness to other tasks
  }
}

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

  xTaskCreate(receiveCommandTask, "receiveCommandTask", 4096, NULL, 1, NULL);
  xTaskCreate(sendLidarDataTask, "sendLidarDataTask", 4096, NULL, 1, NULL);
}

void loop()
{
  // The loop can be left empty as most of the work is done in tasks
  vTaskDelay(1000 / portTICK_PERIOD_MS);  // Adjust delay as needed
}
