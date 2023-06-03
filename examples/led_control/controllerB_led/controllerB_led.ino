#include <SerialBus.h>


#define DEVICE_ADDR 10//this device address
#define PIN_TXRX 4
#define USE_INTERNAL_PULLUP true //enable internal pullup resistor

SerialBus sb(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);


void setup()
{
	// put your setup code here, to run once:
	
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(9600);	

	while(!sb.begin()) //1000 baud with internal pullup
	{
		Serial.println("SerialBus begin() failed.");
		delay(1000);
	}

}

void loop()
{
	// put your main code here, to run repeatedly:

  int buttonPressed = sb.read();
	digitalWrite(LED_BUILTIN, buttonPressed);
	delay(100);

}
