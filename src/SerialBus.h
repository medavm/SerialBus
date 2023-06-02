




#pragma once



#include "Arduino.h"
#include "CBuffer.h"


#define _SB_ 0x01
#define _SB_VERSION 0.1
// #define _SB_DEBUG 1




#define _SB_BUFFER_LEN 64
#define _SB_WRITE_TIMEOUT_MS 1200

#define _SB_INT_SYNC_OFFSET_MAX_US 20

#define _SB_BAUD_MAX 5000 //==100us interrupt interval, 8-bit avr reliable limit?
#define _SB_BAUD_MIN 500 //prevents timer from looping around and generating unintended interrupts
#define _SB_BAUD_DEFAULT 2000

#define _SB_DATA_LEN 8
#define _SB_FRAME_INTERVAL 15
#define _SB_MODE_TX 10
#define _SB_MODE_RX 20
#define _SB_DATA_POS_START 100
#define _SB_DATA_POS_STOP 110




class SerialBus : public Stream
{
private:

    uint8_t _deviceaddr = 0;
    uint8_t _rxtxPin = 0;
    uint8_t _usepull = 0;
    int _baudrate = _SB_BAUD_DEFAULT;

    uint8_t _txbuffer[_SB_BUFFER_LEN];
    uint8_t _rxbuffer[_SB_BUFFER_LEN];
    CBuffer _txcbuffer = CBuffer(_txbuffer, sizeof(_txbuffer));
    CBuffer _rxcbuffer = CBuffer(_rxbuffer, sizeof(_rxbuffer));
    // CBuffer _txcbuffer = CBuffer(_SB_BUFFER_LEN);
    // CBuffer _rxcbuffer = CBuffer(_SB_BUFFER_LEN);

    volatile uint8_t _begin = 0;
    volatile uint8_t _edge = 0;
    volatile uint8_t _mode = 0;
    volatile uint8_t _txstate = 0;
    volatile uint8_t _databuf = 0;
    volatile uint8_t _databufpos = 0;
    volatile uint8_t _datacounter = 0;

    volatile uint32_t _timerinterval_us = 0;
    volatile uint32_t _lastpinchange_us = 0;
    volatile uint32_t _lasttx_us = 0;


    int  hwInit();
    void timerIntStart(uint32_t interval_us, bool restart=false);
    void timerIntStop();
    void timerIntOffset(int offset_us);

    void pcIntEnable();
    void pcIntDisable();

    void busWrite(uint8_t bit);
    bool busRead();

public:
    SerialBus(uint8_t unique_device_addr, uint8_t rxtxPin, uint8_t pullup);
    ~SerialBus();

    int available();
    int read();
    int peek();
    size_t write(uint8_t b);
    void flush();

    void setBaud(int baud);

    int begin();
    int end();
    int write(uint8_t* data, uint8_t datalen);

    

    //interrupt handlers, not meant to be called externally
    void __isr_timer();
    void __isr_pcint();

};



#if defined(__AVR_ATmega328P__)
    #define _SB_DEBUG_OUTPUT_1_PIN 13

    #include "AVR-imp.h"

#elif defined(__AVR_ATmega2560__)
    #define _SB_DEBUG_OUTPUT_1_PIN 10

    #include "AVR-imp.h"

#elif defined(__AVR_ATtiny88__)
    #define _SB_DEBUG_OUTPUT_1_PIN 10

    #include "AVR-imp.h"

#elif defined(__AVR_ATtiny85__)
    #define _SB_DEBUG_OUTPUT_1_PIN 1

    #include "AVR-imp.h"

#else
    #error "No compatible controller defined?"
#endif



#include "SerialBus-imp.h"
