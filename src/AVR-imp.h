/**
 * 
 * AVR impl
 *
 * Uses timer0 (same used to provide the "millis()" function ) compare unit A to generate the precision interrupts. PWM functions provided by 
 * this timer no longer work. The timer should be already running and is setup with a 64 clock div. 
 * 
 * For pin change interrupts uses PCINT2_vect as default.
 * 
 * 
 */



#pragma once


#include "SerialBus.h"


#define _SBIT(REG, BIT)     (REG |= (1<<BIT))
#define _CBIT(REG, BIT)     (REG &= ~(1<<BIT))

#define _SB_TIMER_BASE_CLOCK F_CPU//16e6
#define _SB_TIMER_CLOCK_DIV 64
#define _SB_TIMER_CLOCK (_SB_TIMER_BASE_CLOCK / _SB_TIMER_CLOCK_DIV)
#define _SB_TICK_MICROS (1.0 / _SB_TIMER_CLOCK * 1e6) //us elasped per timer tick
#define _SB_TIMER_START_OFFSET -8

#ifdef CTC0 //attiny?
#define _SB_TIMER_MODE_NORMAL()           (_CBIT(TCCR0A, CTC0)) //
#else
#define _SB_TIMER_MODE_NORMAL()           (_CBIT(TCCR0A, WGM02), _CBIT(TCCR0A, WGM01), _CBIT(TCCR0A, WGM00))
#endif

#define _SB_TIMER_COUNT_GET()             (   TCNT0       )   

#define _SB_TIMER_COMPARE_INT   TIMER0_COMPA_vect 
#define _SB_TIMER_COMPARE_INT_ENABLE()          (   _SBIT(TIMSK0, OCIE0A)   )
#define _SB_TIMER_COMPARE_INT_DISABLE()         (   _CBIT(TIMSK0, OCIE0A)   )
#define _SB_TIMER_COMPARE_INT_CLEAR()           (   _SBIT(TIFR0, OCF0A)     )
#define _SB_TIMER_COMPARE_MATCH_GET()           (   OCR0A       )
#define _SB_TIMER_COMPARE_MATCH_SET(VAL)        (   OCR0A=VAL   )


volatile uint8_t _sb_bitmask;
volatile uint8_t* _sb_portmode;
volatile uint8_t* _sb_portinput;
volatile uint8_t* _sb_portoutput;

volatile int8_t  _sb_int_offset_ticks;
volatile uint8_t _sb_int_interval_ticks;
volatile uint8_t _sb_timer_running;

extern SerialBus* _sb_isr_cb_obj;

ISR(_SB_TIMER_COMPARE_INT)
{
    _sb_isr_cb_obj->__isr_timer();

    _SB_TIMER_COMPARE_MATCH_SET( _SB_TIMER_COMPARE_MATCH_GET() + _sb_int_interval_ticks + _sb_int_offset_ticks ); //shedules next interrupt
    _sb_int_offset_ticks = 0;
}



#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
    _sb_isr_cb_obj->__isr_pcint();
}
#elif defined(PCINT0_vect) //attiny?
ISR(PCINT0_vect)
{
    _sb_isr_cb_obj->__isr_pcint();
}
#else
#error "No PCINT vector found"
#endif


int SerialBus::hwInit()
{

    if(_usepull)
    {
        pinMode(_rxtxPin, INPUT_PULLUP);    
    }
    else
    {
        pinMode(_rxtxPin, INPUT);
    }

    uint8_t port = digitalPinToPort(_rxtxPin);
    _sb_bitmask = digitalPinToBitMask(_rxtxPin);
    _sb_portmode = portModeRegister(port);
    _sb_portinput = portInputRegister(port);
    _sb_portoutput = portOutputRegister(port);

    _SB_TIMER_MODE_NORMAL();

    return 1;

}

void SerialBus::timerIntStart(uint32_t interval_us, bool restart)
{
    if(!restart && _sb_timer_running)
    {
        return;
    }
    
    _sb_int_interval_ticks = interval_us / _SB_TICK_MICROS;
    _SB_TIMER_COMPARE_MATCH_SET( _SB_TIMER_COUNT_GET() + _sb_int_interval_ticks + _SB_TIMER_START_OFFSET );
    _SB_TIMER_COMPARE_INT_CLEAR();
    _SB_TIMER_COMPARE_INT_ENABLE();

    _sb_timer_running = true;
}

void SerialBus::timerIntStop()
{
    _SB_TIMER_COMPARE_INT_DISABLE();
    _sb_timer_running = false;
}

void SerialBus::timerIntOffset(int offset_us)
{
    _sb_int_offset_ticks = (offset_us / _SB_TICK_MICROS);
}

void SerialBus::pcIntEnable()
{

    *digitalPinToPCICR(_rxtxPin) |= _BV(digitalPinToPCICRbit(_rxtxPin));
    *digitalPinToPCMSK(_rxtxPin) |= _BV(digitalPinToPCMSKbit(_rxtxPin));
    
}

void SerialBus::pcIntDisable()
{

    *digitalPinToPCICR(_rxtxPin) &= ~_BV(digitalPinToPCICRbit(_rxtxPin));
    *digitalPinToPCMSK(_rxtxPin) &= ~_BV(digitalPinToPCMSKbit(_rxtxPin));
}

void SerialBus::busWrite(uint8_t bit)
{
    if(bit)
    {
        *_sb_portmode &= ~_sb_bitmask; //INPUT
        if(_usepull)
        {
            *_sb_portoutput |= _sb_bitmask; //HIGH
        }
    }
    else
    {
        *_sb_portoutput &= ~_sb_bitmask; //LOW
        *_sb_portmode |= _sb_bitmask; //OUTPUT
    }
}
 
bool SerialBus::busRead()
{
    return (*_sb_portinput & _sb_bitmask) != 0;
}

