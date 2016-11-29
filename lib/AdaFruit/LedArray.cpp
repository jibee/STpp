#include <AdaFruit/LedArray.hpp>
#include <Irq.h>

using namespace AdaFruit;

LedArray::LedArray(
		   Platform::Timer& hwTimer, Platform::Spi& spi, Platform::Gpio& LAT, Platform::Gpio& OE,
		    Platform::Gpio& A, Platform::Gpio& B, Platform::Gpio& C, Platform::Gpio& D
):
    m_hwTimer(hwTimer), m_spi(spi), m_LAT(LAT), m_OE(OE), m_A(A), m_B(B), m_C(C), m_D(D),
m_current_pixel_weight(0), 
m_current_scanline(0),
m_current_pseudo_pwm_counter(0) 
{
    setOutputGPIO(m_LAT);
    setOutputGPIO(m_OE);
    setOutputGPIO(m_A);
    setOutputGPIO(m_B);
    setOutputGPIO(m_C);
    setOutputGPIO(m_D);
    // We need to send 192 bytes 100 000 times per second - this means 20Mbit/second!
    // With some slack to let the interrupt handler operate I reckon we should target
    // around 1MBit / second
    // Peripheral clock is 48Mhz
    m_spi
	.master()
	.setDivisorPow2(2)
	.setLsbFirst(true)
	.enable();
}

void LedArray::interruptHandler()
{
    m_current_pseudo_pwm_counter--;
    if(m_current_pseudo_pwm_counter<=0)
    {
	m_current_pseudo_pwm_counter = 1<<(m_current_pixel_weight-SKIP_LOW_WEIGHT_BITS);
	disableOutput();
	latch();
	updateScanLine();
	enableOutput();
	shiftNextScanline();
    }
}

void LedArray::disableOutput()
{
    // I am assuming OE polarity is active low
    m_OE=true;
}

void LedArray::enableOutput()
{
    m_OE=false;
}

void LedArray::latch()
{
    m_LAT=true;
    m_LAT=false;
}

void LedArray::updateScanLine()
{
    m_A=m_current_scanline&1<<0;
    m_B=m_current_scanline&1<<1;
    m_C=m_current_scanline&1<<2;
    m_D=m_current_scanline&1<<3;
}

void LedArray::shiftNextScanline()
{
    m_current_scanline++;
    if(m_current_scanline>=SCANLINES)
    {
	m_current_scanline=0;
	m_current_pixel_weight++;
	if(m_current_pixel_weight>=BIT_PER_PIXEL)
	{
	    m_current_pixel_weight=SKIP_LOW_WEIGHT_BITS;
	}
    }
    // initiate the DMA transfer now
    m_spi.send(m_datalines[m_current_pixel_weight][m_current_scanline], BYTES_PER_SCANLINE);
}

void LedArray::setOutputGPIO(Platform::Gpio& pin)
{
    pin
	.setDirection(Platform::Gpio::OUTPUT)
	.setSpeed(Platform::Gpio::SPEED_100MHz)
	.setFunction(Platform::Gpio::GPIO)
	.setPushPull();
}

void LedArray::blank()
{
    fill(0x000000);
}

void LedArray::enable()
{
    m_hwTimer
    // We need to be interrupted 102 400 times per second (25 fps, 256 PWM levels, 16 scanlines)
	.setPrescaler(1<<SKIP_LOW_WEIGHT_BITS)
	.setAutoReload(469)
	.setCounter(469)
	.setUIE(true)
	.setURS(true)
	.setOneShot(false)
	.setTopCB([this](int){this->interruptHandler();})
	.enable();
    Platform::Irq(m_hwTimer.irqNr()).setPriority(15).enable();
}

void LedArray::fill(uint32_t color)
{
    for(uint8_t i=0; i<32; ++i)
    {
	horizontalLine(color, i);
    }
}

void LedArray::horizontalLine(uint32_t color, uint8_t line)
{
    uint8_t scanline = line & (SCANLINES-1);
    uint8_t foldline = (line & (SCANLINES))?0:12;
    color=color>>SKIP_LOW_WEIGHT_BITS;
    for(int i=SKIP_LOW_WEIGHT_BITS; i<BIT_PER_PIXEL; ++i)
    {
	for(int b=0; b<4; ++b)
	    if(color & 0x000001)
		m_datalines[i][scanline][b+foldline]=0xff;
	    else
		m_datalines[i][scanline][b+foldline]=0x00;
	for(int b=4; b<8; ++b)
	    if(color & 0x000100)
		m_datalines[i][scanline][b+foldline]=0xff;
	    else
		m_datalines[i][scanline][b+foldline]=0x00;
	for(int b=8; b<12; ++b)
	    if(color & 0x010000)
		m_datalines[i][scanline][b+foldline]=0xff;
	    else
		m_datalines[i][scanline][b+foldline]=0x00;
	color = color>>1;
    }
}

void LedArray::setPixelAt(uint32_t color, uint8_t line, uint8_t column)
{
    //assert(line<SCANLINES*2);
    //ASSERT(column<32);

    uint8_t scanline = line & (SCANLINES-1);
    uint8_t foldline = (line & (SCANLINES))?0:12;
    char bitMask = 1<<(column & 7);
    uint8_t b = column>>3;

    color=color>>SKIP_LOW_WEIGHT_BITS;
    for(int i=SKIP_LOW_WEIGHT_BITS; i<8; ++i)
    {
	if(color & 0x000001)
	    m_datalines[i][scanline][b+foldline]|=bitMask;
	else
	    m_datalines[i][scanline][b+foldline]&=~bitMask;
	if(color & 0x000100)
	    m_datalines[i][scanline][4+b+foldline]|=bitMask;
	else
	    m_datalines[i][scanline][4+b+foldline]&=~bitMask;
	if(color & 0x010000)
	    m_datalines[i][scanline][8+b+foldline]|=bitMask;
	else
	    m_datalines[i][scanline][8+b+foldline]&=~bitMask;
	color = color>>1;
    }
}
/*
void LedArray::setPixelAt(uint32_t color, uint8_t line, uint8_t column)
{
    ASSERT(line<SCANLINES*2);
    ASSERT(column<32);

    uint8_t scanline = line & 7;
    uint8_t foldline = (line & 8)?16:0;
    char bitMask = 1<<(column & 7);

}
*/


