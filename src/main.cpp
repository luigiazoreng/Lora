/*
 * LoRa E22
 * send a transparent message, you must check that the transmitter and receiver have the same
 * CHANNEL ADDL and ADDH
 *
 * Pai attention e22 support RSSI, if you want use that functionality you must enable RSSI on configuration
 * configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
 *
 * and uncomment #define ENABLE_RSSI true in this sketch
 *
 * Renzo Mischianti <https://www.mischianti.org>
 * https://www.mischianti.org/category/my-libraries/ebyte-lora-e22-devices/
 *
 * E22		  ----- esp32
 * M0         ----- 19 (or GND)
 * M1         ----- 21 (or GND)
 * RX         ----- TX2 (PullUP)
 * TX         ----- RX2 (PullUP)
 * AUX        ----- 15  (PullUP)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include "LoRa_E22.h"
#include "Lidar.h"
#include "Motor.h"


#define ENABLE_RSSI true
#define FREQUENCY_915
#define RX1 27
#define TX1 14


Lidar lidar;
Motor motor;
// ---------- esp32 pins --------------
LoRa_E22 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1

// LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
//  -------------------------------------

void setup()
{
	motor.set();
	Serial.begin(115200);
	Serial1.begin(115200, SERIAL_8N1, RX1, TX1);
	delay(500);

	// Startup all pins and UART
	e22ttl.begin();

	//  If you have ever change configuration you must restore It
	ResponseStructContainer c;
	c = e22ttl.getConfiguration();
	Configuration configuration = *(Configuration *)c.data;
	Serial.println(c.status.getResponseDescription());
	configuration.CHAN = 0x17;
	configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
	e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

	Serial.println("Hi, I'm going to send message!");

	// Send message
	ResponseStatus rs = e22ttl.sendMessage("Hello, I'm the transmitter, the robot ");
	// Check If there is some problem of succesfully send
	Serial.println(rs.getResponseDescription());
}

void loop()
{
	// If something available
	if (e22ttl.available() > 1)
	{
		// read the String message
#ifdef ENABLE_RSSI
		ResponseContainer rc = e22ttl.receiveMessageRSSI();
#else
		ResponseContainer rc = e22ttl.receiveMessage();
#endif
		// Is something goes wrong print error
		if (rc.status.code != 1)
		{
			Serial.println(rc.status.getResponseDescription());
		}
		else
		{
			// Print the status received
			// Serial.println(rc.status.getResponseDescription());
			// Print the message received
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
			default:
				motor.stop();
				break;
			}

#ifdef ENABLE_RSSI
			// Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
		}
	}
	if (Serial.available() > 0)
	{
		Serial.println("sending data...");
		byte rxBuffer[2048] = {};
		int rxLength = 0;
		try
		{
			rxLength = Serial.read(rxBuffer, 2048);
		}
		catch (const std::exception &e)
		{
			Serial.println("Error reading data");
		}

		String message = lidar.createString(rxBuffer, rxLength);
		
		e22ttl.sendMessage(message);
	}
}