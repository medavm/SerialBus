#include "/home/mont/Documents/PlatformIO/Projects/WireBus/WireBus/src/WireBus.h"

#define PIN_TXRX 4 //bus wire connected to pin 4
#define DEVICE_ADDR 100 //this device address is 100
#define USE_INTERNAL_PULLUP false //Disabled internal pullups! Use only esp32 pullups to keep bus at 3.3V

WireBus _wirebus(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);

void setup(){
	Serial.begin(9600);
	
	while(!_wirebus.begin()){
		Serial.println("Wirebus begin() failed.");
		delay(1000);
	}
}

void loop(){
	_wirebus.println("Hello there!");
	delay(1000);
}