


#pragma once


#include "SerialBus.h"



#define _SB_TIMER_BASE_CLOCK F_CPU
#define _SB_TIMER_CLOCK_DIV (_SB_TIMER_BASE_CLOCK / 1e6)
#define _SB_TIMER_CLOCK (_SB_TIMER_BASE_CLOCK / _SB_TIMER_CLOCK_DIV)
#define _SB_TIMER_TICK_MICROS (1.0 / _SB_TIMER_CLOCK * 1e6)
#define _SB_TIMER_START_OFFSET -20
#define _SB_TIMER_COMPARE_CHANNEL 1
#define _SB_TIMER TIM3

HardwareTimer _sb_timer(_SB_TIMER);

volatile int16_t  _sb_int_offset_ticks;
volatile uint16_t _sb_int_interval_ticks;
volatile uint16_t _sb_compare_value_ticks;
volatile uint8_t  _sb_timer_running;

extern SerialBus* _sb_isr_cb_obj;

void __sb_timer_isr()
{ 
    _sb_isr_cb_obj->__isr_timer();

    _sb_compare_value_ticks = _sb_compare_value_ticks + _sb_int_interval_ticks + _sb_int_offset_ticks;
    _sb_timer.setCaptureCompare(_SB_TIMER_COMPARE_CHANNEL, _sb_compare_value_ticks, TICK_COMPARE_FORMAT);

    _sb_int_offset_ticks = 0;
}

void __sb_pcint_isr()
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

    _sb_timer.setPrescaleFactor(_SB_TIMER_CLOCK_DIV);
    _sb_timer.setMode(_SB_TIMER_COMPARE_CHANNEL, TIMER_DISABLED); //no output, only interrupt
	_sb_timer.pause();

    return 1;
}

void SerialBus::timerIntStart(uint32_t interval_us, bool restart)
{
    if(!restart && _sb_timer_running)
    {
        return;
    }

    _sb_int_interval_ticks = (interval_us / _SB_TIMER_TICK_MICROS);
    _sb_compare_value_ticks = _sb_int_interval_ticks + _SB_TIMER_START_OFFSET;

    _sb_timer.pause();
    _sb_timer.setCount(0);
    _sb_timer.setCaptureCompare(_SB_TIMER_COMPARE_CHANNEL, _sb_compare_value_ticks, TICK_COMPARE_FORMAT);
    _sb_timer.attachInterrupt(_SB_TIMER_COMPARE_CHANNEL, __sb_timer_isr); //TODO: need to reset int flag?
    _sb_timer.resume();
 
    _sb_timer_running = 1;
}

void SerialBus::timerIntStop()
{
    _sb_timer.pause();
    _sb_timer_running = 0;
}

void SerialBus::timerIntOffset(int offset_us)
{
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
        pinMode(_rxtxpin, INPUT_PULLDOWN);
        pinMode(_rxtxpin, OUTPUT);
    }
}
 
bool SerialBus::busRead()
{
    return digitalRead(_rxtxpin);
}


