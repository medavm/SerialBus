#include <SerialBus.h>


#define DEVICE_ADDR 10//this device address
#define PIN_TXRX 4
#define USE_INTERNAL_PULLUP true //enable internal pullup resistor

SerialBus sb(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);

void setup()
{
	// put your setup code here, to run once:

	Serial.begin(9600);
	Serial.println("");
	delay(100);
	
	while(!sb.begin())
	{
		Serial.println("SerialBus begin() failed.");
		delay(1000);
	}

	delay(100);
}

void loop()
{
	// put your main code here, to run repeatedly:

	static uint32_t lastMsg;
	if(millis() - lastMsg > 1000) //Send message every 1000ms
	{
		sb.println("Msg sent from controller "+String(DEVICE_ADDR) + "!");
		lastMsg = millis();
	}
	
	
	if(sb.available())
	{
		while (sb.available())
		{
			int c = sb.read();
			Serial.print((char)c); 
		}
	}

}