#include "Lidar.h"
#include "Arduino.h"

Lidar::Lidar()
{
	// Constructor implementation
}

void Lidar::printHexa(String req)
{
	// Method implementation
	// Determine the size of the byte array based on the length of the received string
	size_t receivedSize = req.length();
	size_t byteArraySize = receivedSize / 2;

	// Create a byte array
	byte receivedByteArray[byteArraySize];
	// Convert the hexadecimal string to a byte array
	for (size_t i = 0; i < byteArraySize; ++i)
	{
		// Extract two characters from the string
		char hex[3] = {req[i * 2], req[i * 2 + 1], '\0'};

		// Convert the substring to a long
		receivedByteArray[i] = strtoul(hex, nullptr, 16);
	}
	for (size_t i = 0; i < byteArraySize; ++i)
	{
		Serial.write(receivedByteArray[i]);
	}
}

String Lidar::createString(byte rxBuffer[2048], int rxLength)
{
	int rxDataPacketLength = 0;
	int index = 0;
	byte rxDataArray[2048] = {};

	for (int i = 0; i < rxLength; i++)
	{
		if (index == 0)
		{
			// Get 1st Sync Flag
			if (rxBuffer[i] == firstFlag)
			{
				// 1st Sync Flag received
				rxDataArray[index++] = rxBuffer[i];
			}
		}
		else if (index == 1)
		{
			// Get 2nd Sync Flag
			if (rxBuffer[i] == secondFlag)
			{
				// 2nd Sync Flag received
				rxDataArray[index++] = rxBuffer[i];
			}
			else
			{
				// Sync failed, restart search
				index = 0;
			}
		}
		else if (index == 2)
		{
			// Get length field
			rxDataPacketLength = rxBuffer[i] + 2;
			rxDataArray[index++] = rxBuffer[i];
		}
		else if (index < rxDataPacketLength)
		{
			// Get rest of the packet bytes
			rxDataArray[index++] = rxBuffer[i];
			// Check for full packet received
			if (index == rxDataPacketLength)
			{
				// Full packet received, get crc from packege (last two bytes)
				// Calculate and check CRC
				int pktCRC = (rxDataArray[index - 2] << 8);
				pktCRC += rxDataArray[index - 1];
				index = 0;
			}
		}
	}
	String sending = "";
	for (int i = 0; i < rxDataPacketLength; ++i)
	{
		if (rxDataArray[i] < 0x10)
		{
			sending.concat("0");
		}
		sending.concat(String(rxDataArray[i], HEX));
	}
	sending.concat("\r\n");
	return sending;
}
