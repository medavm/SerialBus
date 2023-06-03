


#pragma once


#include "SerialBus.h"


#define _SB_TIMER_BASE_CLOCK 80e6
#define _SB_TIMER_CLOCK_DIV 80
#define _SB_TIMER_CLOCK (_SB_TIMER_BASE_CLOCK / _SB_TIMER_CLOCK_DIV)
#define _SB_TIMER_TICK_MICROS (1.0 / _SB_TIMER_CLOCK * 1e6) //us per timer tick
#define _SB_TIMER_START_OFFSET -17 //
hw_timer_t* _sb_timer;


volatile uint8_t  _sb_last_output_state;
volatile uint8_t  _sb_timer_running;
volatile int16_t  _sb_int_offset_ticks;
volatile uint32_t _sb_int_interval_ticks;
volatile uint32_t _sb_compare_value_ticks;

extern SerialBus* _sb_isr_cb_obj;


void IRAM_ATTR __sb_timer_isr()
{ 
    _sb_isr_cb_obj->__isr_timer();

    _sb_compare_value_ticks = _sb_compare_value_ticks + _sb_int_interval_ticks + _sb_int_offset_ticks;
    timerAlarmWrite(_sb_timer, _sb_compare_value_ticks, false);
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

    _sb_last_output_state=1;

    _sb_timer = timerBegin(3, _SB_TIMER_CLOCK_DIV, true); //
	timerAttachInterrupt(_sb_timer, &__sb_timer_isr, true);
	
    return 1; //
}

void SerialBus::timerIntStart(uint32_t interval_us, bool restart)
{
    if(!restart && _sb_timer_running)
    {
        return;
    }

    _sb_int_interval_ticks = (interval_us / _SB_TIMER_TICK_MICROS);
    _sb_compare_value_ticks = _sb_int_interval_ticks + _SB_TIMER_START_OFFSET;

    timerWrite(_sb_timer, 0);
    timerAlarmWrite(_sb_timer, _sb_compare_value_ticks, false); //
    timerAlarmEnable(_sb_timer);
    timerStart(_sb_timer);

    _sb_timer_running = 1;
}

void SerialBus::timerIntStop()
{
    timerStop(_sb_timer);
    timerAlarmDisable(_sb_timer);
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

