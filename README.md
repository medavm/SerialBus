


## SerialBus
This is an Arduino library that implements a **serial-like protocol** to allow **communication between microcontrollers** using **one single wire** as bus. 

#### More
- **No external hardware** needed, just one wire.
- Can connect **different logic levels MCUs**  by managing the pullups.
- There is **no master** and **no packets**, a device writes a **continuous stream of bytes** until a stop bit. 
- All devices have an **unique** address assigned, used to resolve collisions.
- **Every message is broadcasted** and all devices in the bus receive it. 
- Up to **5000 baud** (to go higher external pullups may be needed).*



#### Compatibility:


Board       |       Pins        |       Other
------------|-------------------|---------------------
Uno         | D2 to D7          |   Uses Timer 0 Compare A and PCINT2
Mega        | D8 to D15         |   Uses Timer 0 Compare A and PCINT2
ATtinyXX    |                   |   Uses Timer 0 Compare A and PCINT0/PCINT2
ESP8266     | ALL GPIO?         |   Uses Timer 1
ESP32       | ALL GPIO          |   Uses Timer 3
STM32       | ALL GPIO          |   Uses Timer 3

<br>

<sup>*Even at 5000 baud can be unstable. This was never made with speed and reliability in mind but rather the accessibility of only having to use one wire. Most ideal for sending **periodic short signal messages**.</sup>

## Installation
Just download the *.zip* file go to your Arduino project and import the library doing:
```
 Sketch -> include Library -> Add .Zip library
```

## Usage and examples


This is a [Stream](https://reference.arduino.cc/reference/en/language/functions/communication/stream/) library so will work just like [Serial](https://reference.arduino.cc/reference/en/language/functions/communication/serial/). The main diference is the need to create the **SerialBus** object which has **tree arguments**:
- The **device address**, a number from **1 to 127** that is **unique** to every device.
- The **pin** where the bus wire is connected.
- Whether to use the controller **internall pullups** resistors . You can disable internal-pullups to connect different logic level mcus, example below.

```
/*
1 is the device address
4 is the pin where the bus wire is connected
true enables internal pullups
*/
SerialBus _serialBus(4, 1, true);
```

#### Examples
[Basic broadcaster and receiver](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/send_receive_msg)

[Button led control](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/led_control)

[Use different logic levels controllers (esp32 and Uno)](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/nano_to_esp32)
 


