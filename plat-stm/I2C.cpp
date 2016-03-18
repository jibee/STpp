#include "I2C.hpp"
#include <stm32f4xx.h>

// INterrupt vectors
/*
31 38 settable I2C1_EV I 2C1 event interrupt 0x0000 00BC
32 39 settable I2C1_ER I2C1 error interrupt 0x0000 00C0
33 40 settable I2C2_EV I2C2 event interrupt 0x0000 00C4 2
34 41 settable I2C2_ER I C2 error interrupt 0x0000 00C8
*/
/* DMA channels
DMA1:
I2C1_RX C1S0
I2C3_RX C3S2
I2C2_RX C7S3 C7S7
I2C1_RX	C1S5
I2C1_TX C1S6 C1S7
*/

using namespace Platform;

static I2C* __i2c_1=nullptr;
static I2C* __i2c_2=nullptr;
static I2C* __i2c_3=nullptr;

extern "C"
{
    void I2C1_EV_IRQHandler(){ if(__i2c_1) __i2c_1->event(); } /* I2C1 Event */
    void I2C1_ER_IRQHandler(){ if(__i2c_1) __i2c_1->error(); } /* I2C1 Error */
    void I2C2_EV_IRQHandler(){ if(__i2c_2) __i2c_2->event(); }/* I2C2 Event */
    void I2C2_ER_IRQHandler(){ if(__i2c_2) __i2c_2->error(); } /* I2C2 Error */
    void I2C3_EV_IRQHandler(){ if(__i2c_3) __i2c_3->event(); } /* I2C3 event */
    void I2C3_ER_IRQHandler(){ if(__i2c_3) __i2c_3->error(); }; /* I2C3 error */
}


I2C::I2C(Gpio& scl, Gpio& sda, I2C_Device d)
{
    // Switch clock on and locate the base register addressp
    switch(d)
    {
        case I2C_1: { m_base = I2C1; RCC->APB1ENR|=1<<21; __i2c_1=this; break; }
        case I2C_2: { m_base = I2C2; RCC->APB1ENR|=1<<22; __i2c_2=this; break; }
        case I2C_3: { m_base = I2C3; RCC->APB1ENR|=1<<23; __i2c_3=this; break; }
        default: while(1){};
    }
    // Initialise the I2C bus controller
    // Control Register 1: Enable device, set to I2C mode
    m_base->CR1 &= 0b1111111111111101;
    m_base->CR1 |= 1<<0;
    // Control Register 2: APB1 clock speed is 42Mhz, set the clock diviser to 0b101010
    // See DM000031020.pdf page 731
    m_base->CR2 = (m_base->CR2 & 0b1111111111100000) | 0b101010;
    // Note: CR2 bit 11 : enable DMA requests
    // bit 10: enable Buffer interrups (and DMA)
    // bit 9: enable event interrupts
// Duty cycle
    // APB1 clock is 48Mhz -> we set the divisor to 240
    m_base->CCR &= 0b0011000000000000;
    m_base->CCR |= 240;
    setAckEnable();
    set7bitMode();
// Own Address - not needed really as we will only act as master.
    setOwnAddress(0x00);
    // Map the pins to the I2C controller
    scl.setAlternate(Gpio::I2C1_3);
    sda.setAlternate(Gpio::I2C1_3);
}

void I2C::set7bitMode()
{
    m_base->OAR1 = m_base->OAR1 & 0x7fffffff;
}
void I2C::setOwnAddress(uint16_t addr)
{
    m_base->OAR1 = (m_base->OAR1 & 0xfffc0000) | ((addr<<1) & 0x0003fffe);
}

uint8_t I2C::getData()
{
    return 0xff & m_base->DR;
}
void I2C::setAddress(uint8_t address, I2C::Direction direction)
{
setData(address<<1 | (direction==Write?0:1));
}

void I2C::setData(uint8_t data)
{
    m_base->DR=data;
}

void I2C::write(uint8_t address, uint8_t* data, int data_count)
{
   /* initiate start sequence */
    generateStart();
    /* check start bit flag. This is required to proceed further*/
    // When Start bit is set interrupt is raised if ITEVFEN is set
    while(!_StartBitIsSet());
    /*send write command to chip*/
    setAddress(address, I2C::Write);
    // Once address is sent ADDR is set
    // Condition is cleared when both registers are read
    // An interrupt is raised when Addr is set
    while(!_AddrBitIsSet());
    /*check master is now in Tx mode*/
    while(!_TransmitModeSet());
    for(int i=0; i<data_count; ++i)
    {
        /*mode register address*/
        setData(data[i]);
        /*wait for byte send to complete*/
        // An interrupt is raised when this happens
        while(!_TxEBitSet());
    }
    /*generate stop*/
    generateStop();
}

void I2C::read(uint8_t address, uint8_t* data, int data_count)
{
    disableInterrupts();
   /* initiate start sequence */
    generateStart();
    /* check start bit flag. This is required to proceed further*/
    // When Start bit is set interrupt is raised if ITEVFEN is set
    while(!_StartBitIsSet());
    /*send write command to chip*/
    setAddress(address, I2C::Read);
    // Once address is sent ADDR is set
    // Condition is cleared when both registers are read
    // An interrupt is raised when Addr is set
    while(!_AddrBitIsSet());
    /*check master is now in Tx mode*/
    while(!_ReceiveModeSet());
    for(int i=0; i<data_count; ++i)
    {
        /*wait for byte send to complete*/
        // An interrupt is raised when this happens
        while(!_RxNEBitSet());
        if(i==data_count-1)
        {
            // Last byte, clear the ACK bit
            clearAckEnable();
        }
        /*mode register address*/
        data[i]=getData();
    }
    /*generate stop*/
    generateStop();
}

void I2C::read_async(uint8_t address, uint8_t* data, int data_count, I2C::ReadCompletionCallback cb)
{
    m_read_cb=cb;
    start_async(address, data, data_count, Read);
}
void I2C::write_async(uint8_t address, uint8_t* data, int data_count, I2C::WriteCompletionCallback cb)
{
    m_write_cb=cb;
    start_async(address, data, data_count, Write);
}

void I2C::start_async(uint8_t address, uint8_t* data, int data_count, Direction dir)
{
    if(Idle==m_activeState)
    {
        enableInterrupts();
        m_activeState=Started;
        m_activeDirection=dir;
        m_address=address;
        m_data=data;
        m_data_position=0;
        m_data_count=data_count;
        setAckEnable();
        generateStart();
    }
}


void I2C::event()
{
    switch(m_activeState)
    {
        case Idle: break;
        case Synchronous: break;
        case Started:
        {
            _StartBitIsSet();
            /*send write command to chip*/
            m_activeState=AddressSent;
            setAddress(m_address, m_activeDirection);
            break;
        }
        case AddressSent:
        {
            // Once address is sent ADDR is set
            // Condition is cleared when both registers are read
            // An interrupt is raised when Addr is set
            _AddrBitIsSet();
            /*check master is now in Tx mode*/
            _ReceiveModeSet();
            m_activeState=DataTransmission;
            if(Write==m_activeDirection)
            {
                setData(m_data[m_data_position]);
                m_data_position++;
            }
            break;
        }
        case DataTransmission:
        {
            if(Read==m_activeDirection)
            {
                if(m_data_position==m_data_count-1)
                {
                    clearAckEnable();
                }
                m_data[m_data_position]=getData();
                m_data_position++;
                if(m_data_position>=m_data_count)
                {
                    generateStop();
                    callReadCompletionCallback();
                    m_activeState=Idle;
                }
            }
            else{
                if(m_data_position>=m_data_count)
                {
                    generateStop();
                    callWriteCompletionCallback();
                    m_activeState=Idle;
                }
                else
                {
                    setData(m_data[m_data_position]);
                    m_data_position++;
                }
            }
        }
    }
}


void I2C::generateStart()
{
    m_base->CR1|=1<<8;    
}
void I2C::generateStop()
{
    m_base->CR1|=1<<9;
}
bool I2C::_StartBitIsSet()
{
    return m_base->SR1&1<<0;
}
bool I2C::_AddrBitIsSet()
{
    return m_base->SR1&1<<1;
}
bool I2C::_TransmitModeSet()
{
    return m_base->SR2&1<<2;
}
bool I2C::_TxEBitSet()
{
    return m_base->SR1&1<<7;
}
bool I2C::_ReceiveModeSet()
{
return !_TransmitModeSet();
}
bool I2C::_RxNEBitSet()
{
    return m_base->SR1&1<<6;
}

void I2C::error()
{

}
void I2C::setAckEnable()
{
// ACK Enable
    m_base->CR1|=1<<10;
}
void I2C::clearAckEnable()
{
    m_base->CR1&=~(1<<10);
}

void I2C::callReadCompletionCallback()
{
    // TODO use a deferred call?
    m_read_cb(m_data, m_data_count);
}
void I2C::callWriteCompletionCallback()
{
    // TODO use a deferred call?
    m_write_cb();
}

void I2C::enableInterrupts()
{
    m_base->CR2|=0b11<<9;
}


void I2C::disableInterrupts()
{
    m_base->CR2&=~(0b11<<9);
}



