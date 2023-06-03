#include "/home/mont/Documents/PlatformIO/Projects/WireBus/WireBus/src/WireBus.h"


//#include <WireBus.h>


#define PIN_NUM 4 //D2 to D7 on Uno/Nano 
#define DEVICE_ADDR 1 //Device address must be unique to every device on the bus! (Range 0 to 255)
WireBus _wirebus(DEVICE_ADDR, PIN_NUM);

void setup()
{
	// put your setup code here, to run once:

	Serial.begin(9600); //Select desire uart baud rate
	Serial.println("");
	delay(100);
	
	while(!_wirebus.begin()) //Starts WireBus at 1000 baud and uses controller internal PULLUP
	{
		Serial.println("Wirebus begin() failed.");
		delay(1000);
	}

	delay(100);
}

void loop()
{
	// put your main code here, to run repeatedly:

	static uint32_t lastMsg;
	if(millis() - lastMsg > 1000) //sends message every 2s
	{
		_wirebus.println("Msg sent from controller "+String(DEVICE_ADDR) + "!");
		lastMsg = millis();
	}
	
	
	if(_wirebus.available())
	{
		while (_wirebus.available())
		{
			int c = _wirebus.read();
			Serial.print((char)c); //Prints data received through wirebus
		}
	}

}