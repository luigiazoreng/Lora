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

#define ENABLE_RSSI true
#define FREQUENCY_915
#include "Arduino.h"
#include "LoRa_E22.h"
#include "Lidar.h"

Lidar lidar;

// ---------- esp32 pins --------------
LoRa_E22 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1

// LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
//  -------------------------------------

void setup()
{
	Serial.begin(115200);
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
	ResponseStatus rs = e22ttl.sendMessage("Hello, I'm the receiver ");
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
			

#ifdef ENABLE_RSSI
			// Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
		}
	}
	if (Serial.available())
	{
		String input = Serial.readString();
		input.concat("\r\n");
		e22ttl.sendMessage(input);
	}
}