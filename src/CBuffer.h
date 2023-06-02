 #pragma once


class CBuffer
{
private:
    /* data */
    bool _dync = false;
    unsigned char* _buffer;
    unsigned char  _capacity;
    volatile unsigned char  _len = 0;
    volatile unsigned char _head = 0;
    volatile unsigned char _tail = 0;

public:
    CBuffer(unsigned char* buffer, unsigned char len);
    CBuffer(unsigned char len);
    //CBuffer();
    ~CBuffer();
    
    int size();
    int available();
    int free();
    int peek();
    int read();
    int write(unsigned char b);
    int clear(); 


};

CBuffer::CBuffer(unsigned char* buffer, unsigned char len)
{
    _buffer = buffer;
    _capacity = len;
}

CBuffer::CBuffer(unsigned char len)
{
    _buffer = new unsigned char[len];
    _capacity = len;
    _dync = true;
}

CBuffer::~CBuffer()
{
    if(_dync)
    {
        delete _buffer;
    }
}

int CBuffer::size()
{
    return _capacity;
}

int CBuffer::available()
{
    return _len;
}

int CBuffer::free()
{
    return  _capacity - _len;
}

int CBuffer::peek()
{
    if (_len == 0) 
    {
        return -1;
    }
    return _buffer[_head];
}

int CBuffer::read()
{
    if (_len == 0) 
    {
        return -1;
    }
    int b = _buffer[_head];
    _len--;
    _head++;

    if (_head == _capacity) 
    {
        _head = 0;
    }
    return b;
}

int CBuffer::write(unsigned char b)
{
    if (_len == _capacity)
    {
        return 0 ;
    }
    _buffer[_tail] = b;
    _len++;

    // _tail = (_tail + 1) % _capacity;
    _tail++;
    if (_tail == _capacity) 
    {
        _tail = 0;
    }

    return 1;

}

int CBuffer::clear()
{
    unsigned char count = 0;
    while (read() > -1)
    {
        count++;
    }
    
    return count;
}