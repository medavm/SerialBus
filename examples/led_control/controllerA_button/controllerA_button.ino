#include <SerialBus.h>


#define PIN_BUTTON  2

#define DEVICE_ADDR 20//this device address
#define PIN_TXRX 4
#define USE_INTERNAL_PULLUP true //enable internal pullup resistor

SerialBus sb(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);

void setup()
{
	// put your setup code here, to run once:

  pinMode(PIN_BUTTON, INPUT_PULLUP);

	Serial.begin(9600);

	while(!sb.begin())
	{
		Serial.println("SerialBus begin() failed.");
		delay(1000);
	}

}

void loop()
{
	// put your main code here, to run repeatedly:

  int buttonPressed = !digitalRead(PIN_BUTTON);
	sb.write(buttonPressed);
	delay(200);

}