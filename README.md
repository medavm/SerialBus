


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
- Whether to use the controller **internall pullups** resistors . You can disable internal-pullups to connect different logic level mcus.

```
/*
1 is the device address
4 is the pin where the bus wire is connected
true enables internal pullups
*/
SerialBus _serialBus(4, 1, true)
```

#### Examples
[Basic broadcaster and receiver](http://example.com)

[Button led control](http://example.com)


## How it works?

A quick explanation... Uses the hardware timer paired with bit bagging to create a transmission protocol. Pin change interrupts are also used to detect the start of a transmission and to keep the receivers synced.

In rest the bus is held at *HIGH* level and a message consist of the following:

    START(1) -> ADDR(8) -> STOP*(1) -> DATA 0(8) -> STOP(1) -> ... -> DATA N(8) -> STOP(1)

- START (1 bit) -> To start the transmission, bus gets pulled *LOW* for 1 bit length.
- ADDR (8 bit) -> The broadcasting device address, this used to solve collisions.
- STOP* (1 bit)  -> This bit has actually no real propose but is here to simplify the code.
- DATA (8 bit) - Byte of data.
- STOP (1 bit)- If oposite from previous data bit indicates another data byte is going to be transmitted; if equals to previous data bit, no more data is going to be transmitted. This way we guarantee the pin changes state at least once for every byte sent, helps to keep the receivers synced with the broadcaster timing. 


