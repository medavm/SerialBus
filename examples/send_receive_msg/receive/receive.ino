#include <SerialBus.h>


#define DEVICE_ADDR 2//this device address
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

    while (sb.available()){
      int c = sb.read();
      Serial.print((char)c);
    }

    delay(10);

}