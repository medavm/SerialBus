#include "/home/mont/Documents/PlatformIO/Projects/WireBus/WireBus/src/WireBus.h"

#define PIN_TXRX 4 //bus wire connected to pin 4
#define DEVICE_ADDR 101 //this device address is 101
#define USE_INTERNAL_PULLUP true //Enable pullups here! Disable the pullups on the 5v voltage devices to keep the bus at 3.3v

WireBus _wirebus(DEVICE_ADDR, PIN_TXRX, USE_INTERNAL_PULLUP);

void setup(){
  Serial.begin(9600);
  
  while(!_wirebus.begin()){ //true to enables mcu internal pulls  
    Serial.println("Wirebus begin() failed.");
    delay(1000);
  }
}

void loop(){
    while (_wirebus.available()){
      int c = _wirebus.read();
      Serial.print((char)c);
    }
    delay(100);
}