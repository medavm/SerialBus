


#pragma once


#include "SerialBus.h"


#define _SB_TIMER_BASE_CLOCK 80e6
// #define _SB_TIMER_CLOCK_DIV 16 //TIM_DIV256
#define _SB_TIMER_CLOCK_DIV 256 //TIM_DIV256
#define _SB_TIMER_CLOCK (_SB_TIMER_BASE_CLOCK / _SB_TIMER_CLOCK_DIV)
#define _SB_TIMER_TICK_MICROS (1.0 / _SB_TIMER_CLOCK * 1e6) //us elasped per timer tick
#define _SB_TIMER_START_OFFSET -6


volatile int16_t  _sb_int_offset_ticks;
volatile uint16_t _sb_int_interval_ticks;
volatile uint8_t  _sb_timer_running;

extern SerialBus* _sb_isr_cb_obj;

void IRAM_ATTR __sb_timer_isr()
{     
    timer1_write(_sb_int_interval_ticks + _sb_int_offset_ticks);
    _sb_int_offset_ticks = 0;
    
    _sb_isr_cb_obj->__isr_timer();
}

void IRAM_ATTR __sb_pcint_isr()
{
    _sb_isr_cb_obj->__isr_pcint();
}

int SerialBus::hwInit()
{
    if(_usepull)
    {
        pinMode(_rxtxpin, INPUT_PULLUP);
    }
    else
    {
        pinMode(_rxtxpin, INPUT);
    }

    timer1_disable();
    timer1_attachInterrupt(__sb_timer_isr);

    return 1;
}

void SerialBus::timerIntStart(uint32_t interval_us, bool restart)
{
    if(!restart && _sb_timer_running)
    {
        return;
    }

    _sb_int_interval_ticks = (interval_us / _SB_TIMER_TICK_MICROS);

    timer1_attachInterrupt(__sb_timer_isr);
    timer1_write(_sb_int_interval_ticks + _SB_TIMER_START_OFFSET);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);

    _sb_timer_running = 1;
}

void SerialBus::timerIntStop()
{
    timer1_disable();
    _sb_timer_running = 0;
}

void SerialBus::timerIntOffset(int offset_us)
{
    if(_sb_int_offset_ticks>0)
        return;
    _sb_int_offset_ticks = (offset_us / _SB_TIMER_TICK_MICROS);
}

void SerialBus::pcIntEnable()
{

    attachInterrupt(_rxtxpin, __sb_pcint_isr, CHANGE);

}

void SerialBus::pcIntDisable()
{
    detachInterrupt(_rxtxpin);
}
 
void SerialBus::busWrite(uint8_t bit)
{
    if(bit)
    {
        if(_usepull)
        {
            pinMode(_rxtxpin, INPUT_PULLUP);
        }
        else
        {
            pinMode(_rxtxpin, INPUT);
        }
    }
    else
    {
        pinMode(_rxtxpin, OUTPUT);
        digitalWrite(_rxtxpin, 0);
    }
        // _sb_int_offset_ticks = -??; //TODO need this for faster speeds?
}
 
bool SerialBus::busRead()
{
    return digitalRead(_rxtxpin);
}

