#include <SerialBus.h>


#ifdef ARDUINO_ARCH_AVR
    #inclugde "AVR-imp.h"
#endif


#define DEVICE_ADDR 1 //this device address
#define PIN_TXRX 4
#define USE_INTERNAL_PULLUP true //enable internal pullup resistor

SerialBus sb(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);

void setup(){

	Serial.begin(9600);
	
	while(!sb.begin()){
		Serial.println("SerialBus begin() failed.");
		delay(1000);
	}

}

void loop(){

	sb.println("Hello there!");

	delay(1000);
	
}