
/**
 * 
 * 
 * 
 * A quick explanation... Uses the hardware timer and pcint to bit bang a simple serial-like protocol:
 * 
 * ...[REST] --> [START] --> [SRC ADDR] --> [STOP*] --> [DATA 0] ... [DATA N] --> [STOP] --> [REST]...
 * 
 * REST -> Bus help at HIGH state.
 * START -> The broadcasting device pulls the bus LOW for 1 bit length.
 * SRC ADDR -> 8 bits contaning the broadcaster device address, this is used to solve collisions.
 * STOP* -> 1 bit. This bit has no real propose but is here to simplify the code.
 * DATA -> 8 bits, byte of actually data payload
 * STOP -> 1 bit. If this is oposite from the previous bit of data indicates another byte of data is going to be transmited. If equals to previous data bit
 * means no more data is comming, signs the end of transmission and the bus should be released and at HIGH state next bit. Doing it this way we guarantee a pcint 
 * at least once for every byte sent, helping to keep the receivers synced with the broadcaster timing.
 * 
 * 
 * 
*/




#pragma once


#include "SerialBus.h"



SerialBus* _sb_isr_cb_obj;
 
SerialBus::SerialBus(uint8_t unique_device_addr, uint8_t pin, uint8_t pullup)
{
    _deviceaddr = unique_device_addr;
    _rxtxpin = pin;
    _usepull = pullup;
    _sb_isr_cb_obj = this;
}

SerialBus::~SerialBus()
{

}

void SerialBus::__isr_timer()
{
    #if _SB_DEBUG
    if(_edge)
        digitalWrite(_SB_DEBUG_OUTPUT_1_PIN, 1);
    #endif

    //if _edge this interrupt happend at the start/end edge of a bit being transmitted (bus is written while transmitting here)
    //if !_edge this interrupt happend in middle of the bit being transmitted (bus gets read while receiving here)
    _edge = !_edge; 

    //When this device is currently transmitting data
    if(_mode == _SB_MODE_TX)
    {

        //Next bus state gets processed here
        if(_edge)
        {

            //Sets bus to rest and leaves the tx mode
            if(_databufpos == _SB_DATA_POS_STOP)
            {
                _mode = 0;
                _txstate = 1;
                _lasttx_us = micros();
            }

            //Stop bit
            //If equals to previous bit tells the receiver to stop. To continue transmitting must change state from previous bit.
            else if (_databufpos > _SB_DATA_LEN - 1)
            {         
                if (_datacounter < _SB_BUFFER_LEN && _txcbuffer.available())
                {
                    _databufpos = 0;
                    _txstate = !(_databuf & 1); //keep transmitting
                }
                else
                {
                    _databufpos = _SB_DATA_POS_STOP;
                    _txstate = (_databuf & 1); //
                }
                _datacounter++;
            }

            //Sets next bit and load next byte to buffer
            else
            {
                if (_databufpos == 0) 
                {
                    _databuf = _datacounter==0?_deviceaddr:_txcbuffer.read();
                }

                _txstate = (_databuf & (1 << (_SB_DATA_LEN - 1 - _databufpos++))) > 0;
            }

            busWrite(_txstate); 
        }



        //Reads the bus state while transmitting here
        else
        {
            //This prevents two devices from trying to transmit at the same time, highest address goes into receive mode
            if (_datacounter==0 && _txstate != busRead())
            {
                _mode = _SB_MODE_RX;
                _databuf = _databuf & (255 << (_SB_DATA_LEN - (_databufpos - 1))); //keep previous and set last bits to 0; TODO?
                pcIntEnable();
            }
        }
    }

    
    //When this device is currently receiving data
    else if(_mode == _SB_MODE_RX)
    {

        if(_edge)
        {

            //Leave rx mode after stop bit
            if (_databufpos == _SB_DATA_POS_STOP)
            {
                _mode = 0;
                _lasttx_us = micros();
            }

        }

        //Reads the bus state and processes data being received here
        else
        {

            //Start bit
            if(_databufpos == _SB_DATA_POS_START)
            { 
                _databuf = 0;
                _databufpos = 0;
                _datacounter = 0;
            }

            //Reads stop bit
            //Loads previous received byte into the buffer
            else if (_databufpos > _SB_DATA_LEN - 1)
            {

                if (_datacounter > 0) //ignore first byte, is the trasmitting device address
                {
                    _rxcbuffer.write(_databuf);
                }
 
                if (busRead() == (_databuf & 1) || _datacounter > _SB_BUFFER_LEN - 1) //If stop bit or ?; TODO
                {
                    _databufpos = _SB_DATA_POS_STOP;
                }
                else
                {
                    _databufpos = 0;
                    _databuf = 0;
                }
                
                _datacounter++;
            }

            //Reads next bit of byte being received
            else
            {
                _databuf = (_databuf << 1) | busRead();
                _databufpos++;
            }


            //This helps keeping the receiver interrupts synced with the transmitting device
            if(_lastpinchange_us)
            {
                int offset_us = _timerinterval_us - (micros() - _lastpinchange_us);
                if(offset_us > _SB_INT_SYNC_OFFSET_MAX_US || offset_us < -_SB_INT_SYNC_OFFSET_MAX_US)
                {
                    timerIntOffset(offset_us);
                }
                _lastpinchange_us = 0; 
            }


        }


    }

    //Not currently transmiting or receiving
    else 
    {

        if(_edge)
        {
            //If busRead()==0 but we not yet in receive mode, means other device just started transming!? We do nothing 'cause shoud have a pcint 
            // ready to put us into receive mode
            if(_txcbuffer.available() && busRead() && micros() - _lasttx_us > _timerinterval_us * 2 *  _SB_FRAME_INTERVAL)
            {
                pcIntDisable(); //TODO: clear pending int flag?
                busWrite(0);
                _txstate = 0;
                _databuf = 0;
                _databufpos = 0;
                _datacounter = 0;
                _mode = _SB_MODE_TX;
            }
          
        }
        else
        {

            if(!_txcbuffer.available())
            {
                timerIntStop();
            }
            
            pcIntEnable(); //will be disabled after a transmission and gets re enabled here
        }

         
    }


     #if _SB_DEBUG
    digitalWrite(_SB_DEBUG_OUTPUT_1_PIN, 0);
    #endif

}

void SerialBus::__isr_pcint()
{

    //Record every pin change. Used to sync interrupt timing in the timer isr
    if (_mode == _SB_MODE_RX && _databufpos != _SB_DATA_POS_STOP) //
    {
        _lastpinchange_us = micros();
    }

    //If bus goes low, starts receiving here
    else if(!_mode && !busRead())
    {
        _mode = _SB_MODE_RX;
        _databufpos = _SB_DATA_POS_START;
        _databuf = 0;
        _datacounter = 0;
        _edge = 1; //
        timerIntStart(_timerinterval_us, true);
    }

}

void SerialBus::setBaud(int baud)
{
    if (baud > _SB_BAUD_MAX)
    {
        baud = _SB_BAUD_MAX;
    }
    else if (baud < _SB_BAUD_MIN)
    {
        baud = _SB_BAUD_MIN;
    }
    _baudrate = baud;

    if(_begin)
    {
        end();
        begin();
    }
    
}

int SerialBus::begin()
{
    if(_begin)
    {
        end();
    }

    #if _SB_DEBUG
    pinMode(_SB_DEBUG_OUTPUT_1_PIN, OUTPUT);
    #endif


    uint32_t _baudinterval_us = 1.0 / _baudrate * 1e6;
    _timerinterval_us = _baudinterval_us / 2; //
    if(!hwInit())
    {
        return 0;
    }
    delay(10);

    //avoid joining the bus in the middle of a transmission
    uint8_t bus_high = 0;
    while (bus_high < _SB_DATA_LEN+2)
    {
        if(busRead())
        {
            bus_high++;
        }
        else
        {
            bus_high=0;
        }

        delayMicroseconds(_baudinterval_us);
    }


    pcIntEnable();
    _begin = 1;

    return 1;
}

int SerialBus::end()
{
    flush();
    pcIntDisable();
    timerIntStop();
    _txcbuffer.clear();
    _rxcbuffer.clear();
    _begin = 0; 
    return 1;
}

int SerialBus::available()
{
    return _rxcbuffer.available();
}

int SerialBus::peek()
{
    return _rxcbuffer.peek();
}

int SerialBus::read()
{
    return _rxcbuffer.read();
}

size_t SerialBus::write(uint8_t b)
{
    if(!_begin)
    {
        return 0;
    }

    uint32_t start = millis();
    while (!_txcbuffer.free() && millis() - start < _SB_WRITE_TIMEOUT_MS)
    {
        delay(1);
    }
    
    if(!_txcbuffer.write(b)) //still not free?
    {
        return 0;
    }

    timerIntStart(_timerinterval_us, false);
       
    return 1;
}

int SerialBus::write(uint8_t* data, uint8_t datalen)
{

    if(datalen > _SB_BUFFER_LEN)
    {
        datalen = _SB_BUFFER_LEN - 1;
    }
        
    for (int i = 0; i < datalen; i++)
    {
        if(!write(data[i]))
        {
            return i;
        }
    }

    return datalen;
}

void SerialBus::flush()
{
    uint32_t start = millis();
    while ( (_txcbuffer.available() || _mode== _SB_MODE_TX) && millis() - start < _SB_WRITE_TIMEOUT_MS)
    {
        delay(1);
    }
}
