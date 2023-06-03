#include <SerialBus.h>


#define DEVICE_ADDR 20 //device address
#define PIN_TXRX 4
#define USE_INTERNAL_PULLUP false //Disabled internal pullups! Use only esp32 pullups to keep bus at 3.3V

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