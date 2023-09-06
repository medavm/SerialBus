


## SerialBus
Arduino library that implements a **serial-like communication** between microcontrollers using **one single wire** as bus.


Board       |       Pins        |       Other
------------|-------------------|---------------------
Uno         | D2 to D7          |   Uses Timer 0 Compare A and PCINT2
Mega        | D8 to D15         |   Uses Timer 0 Compare A and PCINT2
ATtinyXX    |                   |   Uses Timer 0 Compare A and PCINT0/PCINT2
ESP8266     | ALL GPIO?         |   Uses Timer 1
ESP32       | ALL GPIO          |   Uses Timer 3
STM32       | ALL GPIO          |   Uses Timer 3

<br>


#### Examples
[Basic broadcaster and receiver](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/send_receive_msg)

[Button led control](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/led_control)

[Use different logic levels controllers (esp32 and Uno)](https://github.com/jgvmonteiro/SerialBus/tree/main/examples/nano_to_esp32)
 


