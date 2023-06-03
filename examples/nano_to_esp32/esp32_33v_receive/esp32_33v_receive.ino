#include <SerialBus.h>


#define DEVICE_ADDR 10 //device address
#define PIN_TXRX 4 
#define USE_INTERNAL_PULLUP true //Enable pullups here! Disable the pullups on the 5v devices to keep the bus at 3.3v!

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