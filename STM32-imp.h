/**
 * AVR.h
 * 
 * WireBus.h implemention for stm32 boards
 *
 * Uses timer0 (same timer used by arduino's "millis" library) output compare unit to generate the precision interrupts. Means PWM functions provided by 
 * this timer no longer work. The timer should be already running and is setup with a 64 clock div. 
 * 
 * For pin change interrupts uses PCINT2_vect as default, so can be used pins D0 to D7 on uno/nano, D8 to D15 on mega.
 * 
 * Tested for only on nano(atmega328) and mega(atmega2560) boards
 * 
 * 
 * 
 * 
 * March 2023 - jgvmonteiro@hotmail.com
 * 
 */





#pragma once


#include "WireBus.h"



#define _WB_TIMER_BASE_CLOCK 72e6
#define _WB_TIMER_CLOCK_DIV 72
#define _WB_TIMER_CLOCK (_WB_TIMER_BASE_CLOCK / _WB_TIMER_CLOCK_DIV)
#define _WB_TIMER_TICK_MICROS (1.0 / _WB_TIMER_CLOCK * 1e6)
#define _WB_TIMER_START_OFFSET -20
#define _WB_TIMER_COMPARE_CHANNEL 1
#define _WB_TIMER TIM1

HardwareTimer _wb_timer(_WB_TIMER);

uint8_t _wb_rxtxPin;
uint8_t _wb_pullup;

volatile int16_t  _wb_int_offset_ticks;
volatile uint16_t _wb_int_interval_ticks;
volatile uint16_t _wb_compare_value_ticks;

volatile uint8_t  _wb_lastbit;
volatile uint8_t  _wb_timer_running;

extern WireBus* _wb_isr_cb_obj;

void __wirebus_timer_isr()
{ 
    _wb_isr_cb_obj->__isr_timer();

    _wb_compare_value_ticks = _wb_compare_value_ticks + _wb_int_interval_ticks + _wb_int_offset_ticks;
    _wb_timer.setCaptureCompare(_WB_TIMER_COMPARE_CHANNEL, _wb_compare_value_ticks, TICK_COMPARE_FORMAT);

    _wb_int_offset_ticks = 0;
}

void __wirebus_pcint_isr()
{
    _wb_isr_cb_obj->__isr_pcint();
}

int WireBus::__HW_INIT(uint8_t rxtxPin, bool usePULLUP)
{

    if(usePULLUP)
    {
        pinMode(rxtxPin, INPUT_PULLUP);
    }
    else
    {
        pinMode(rxtxPin, INPUT);
    }

    _wb_lastbit=1;
    _wb_pullup = usePULLUP;
    _wb_rxtxPin = rxtxPin;

    _wb_timer.setPrescaleFactor(_WB_TIMER_CLOCK_DIV);
    _wb_timer.setMode(1, TIMER_DISABLED); //no output, only interrupt
	_wb_timer.pause();

    return 1;
}

void WireBus::__HW_TIMER_START(uint32_t interval_us, bool restart)
{
    if(!restart && _wb_timer_running)
    {
        return;
    }

    _wb_int_interval_ticks = (interval_us / _WB_TIMER_TICK_MICROS);
    _wb_compare_value_ticks = _wb_int_interval_ticks + _WB_TIMER_START_OFFSET;

    _wb_timer.pause();
    _wb_timer.setCount(0);
    _wb_timer.setCaptureCompare(_WB_TIMER_COMPARE_CHANNEL, _wb_compare_value_ticks, TICK_COMPARE_FORMAT);
    _wb_timer.attachInterrupt(_WB_TIMER_COMPARE_CHANNEL, __wirebus_timer_isr); //TODO: reset int flag?
    _wb_timer.resume();
 
    _wb_timer_running = 1;
}

void WireBus::__HW_TIMER_STOP()
{
    _wb_timer.pause();
    _wb_timer_running = 0;
}

void WireBus::__HW_TIMER_OFFSET(int offset_us)
{
    _wb_int_offset_ticks = (offset_us / _WB_TIMER_TICK_MICROS);
}

void WireBus::__PINCHANGE_INT_ENABLED(bool enable)
{
    if (enable)
    {
        attachInterrupt(_wb_rxtxPin, __wirebus_pcint_isr, CHANGE);
    }
    else
    {
        detachInterrupt(_wb_rxtxPin);
    }
}
 
void WireBus::__BUS_STATE_SET(uint8_t bit)
{
    if(bit)
    {
        if(!_wb_lastbit)
        {
            if(_wb_pullup)
            {
                pinMode(_wb_rxtxPin, INPUT_PULLUP);
            }
            else
            {
                pinMode(_wb_rxtxPin, INPUT);
            }
        }
    }
    else
    {
        if(_wb_lastbit)
        {
            pinMode(_wb_rxtxPin, INPUT_PULLDOWN);
            pinMode(_wb_rxtxPin, OUTPUT);
        }
    }

    _wb_lastbit = bit;
}
 
bool WireBus::__BUS_STATE_GET()
{
    return digitalRead(_wb_rxtxPin);
}































// #include <Arduino.h>

// #define __TIMER_BASE_CLOCK 72e6
// #define __TIMER_CLOCK_DIV 72
// #define __TIMER_CLOCK (__TIMER_BASE_CLOCK / __TIMER_CLOCK_DIV)
// #define __TIMER_TICK_MICROS (1.0 / __TIMER_CLOCK * 1e6) //microseconds elasped per timer tick
// #define __TIMER_START_OFFSET -20 //compensation for isr execution delay
// HardwareTimer _timer(TIM1);




// uint8_t _pin;
// uint8_t _usepullup;

// volatile uint16_t _int_interval;
// volatile uint16_t _wb_compare_value_ticks;
// volatile int16_t  _wb_int_offset_ticks;
// volatile uint8_t  _laststate;
// volatile uint8_t  _timer_running;
 

// extern WireBus* __wirebus_isr_cb_obj;

// void __wirebus_timer_isr()
// { 
//     __wirebus_isr_cb_obj->__isr_timer();

//     _wb_compare_value_ticks = _wb_compare_value_ticks + _int_interval + _wb_int_offset_ticks;
//     _timer.setCaptureCompare(1, _wb_compare_value_ticks, TICK_COMPARE_FORMAT);
//     _wb_int_offset_ticks = 0;
// }

// void __wirebus_pcint_isr()
// {
//     __wirebus_isr_cb_obj->__isr_pcint();
// }

// int WireBus::hwInit(uint8_t pin, bool usepullup)
// {

//     if(usepullup)
//     {
//         pinMode(pin, INPUT_PULLUP);
//     }
//     else
//     {
//         pinMode(pin, INPUT);
//     }


//     _laststate=1;
//     _usepullup = usepullup;
//     _pin = pin;

//     _timer.setPrescaleFactor(__TIMER_CLOCK_DIV), //divide clock 72 to make it 1mghz counter
//     _timer.setMode(1, TIMER_DISABLED);
//     _timer.attachInterrupt(1, __wirebus_timer_isr);
// 	_timer.pause();


//     return 1; //todo check if valid pin
// }

// void WireBus::timerStart(uint32_t interval_us, bool restart)
// {
//     if(!restart && _timer_running)
//     {
//         return;
//     }

//     _int_interval = (interval_us / __TIMER_TICK_MICROS);
//     _wb_compare_value_ticks = _int_interval + __TIMER_START_OFFSET;

//     _timer.pause();
//     _timer.setCount(0);
//     _timer.setCaptureCompare(1, _wb_compare_value_ticks, TICK_COMPARE_FORMAT);
//     _timer.attachInterrupt(1, __wirebus_timer_isr); //todo: reset int flag
//     _timer.resume();
//     _timer_running = 1;
// }

// void WireBus::timerStop()
// {
//     _timer.pause();
//     _timer_running = 0;
// }

// void WireBus::timerOffset(int offset_us)
// {
//     _wb_int_offset_ticks = (offset_us / __TIMER_TICK_MICROS);
// }

// void WireBus::enablePCINT(bool enable)
// {
//     if (enable)
//     {
//         attachInterrupt(_pin, __wirebus_pcint_isr, CHANGE);
//     }
//     else
//     {
//         detachInterrupt(_pin);
//     }
// }
 
// void WireBus::setBus(bool state)
// {
//     if(state)
//     {
//         if(!_laststate)
//         {
//             if(_usepullup)
//             {
//                 pinMode(_pin, INPUT_PULLUP);
//             }
//             else
//             {
//                 pinMode(_pin, INPUT);
//             }
//         }
//         _laststate = 1;
//     }
//     else
//     {
//         if(_laststate==1)
//         {
//             pinMode(_pin, INPUT_PULLDOWN);
//             pinMode(_pin, OUTPUT);
//         }
//         _laststate = 0;
//     }
// }
 
// bool WireBus::getBus()
// {
//     return digitalRead(_pin);
// }



// #if _WIREBUS_DEBUG
// uint8_t _output1_state;
// uint8_t _output2_state;
// uint8_t _output3_state;

// void __WIREBUS_DEBUG_OUTPUT_1_SET(uint8_t state)
// {
//     if(state == _WIREBUS_DEBUG_TOGGLE)
//     {
//         _output1_state = !_output1_state;
//         digitalWrite(_WIREBUS_DEBUG_OUTPUT_1_PIN, _output1_state);
//     }
//     else
//     {
//         digitalWrite(_WIREBUS_DEBUG_OUTPUT_1_PIN, state);
//         _output1_state = state;
//     }
    
// }

// void __WIREBUS_DEBUG_OUTPUT_2_SET(uint8_t state)
// {
//     if(state == _WIREBUS_DEBUG_TOGGLE)
//     {
//         _output2_state = !_output2_state;
//         digitalWrite(_WIREBUS_DEBUG_OUTPUT_2_PIN, _output2_state);
//     }
//     else
//     {
//         digitalWrite(_WIREBUS_DEBUG_OUTPUT_2_PIN, state);
//         _output2_state = state;
//     }
// }
// #endif




