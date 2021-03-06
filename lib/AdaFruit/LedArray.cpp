#include <AdaFruit/LedArray.hpp>

using namespace AdaFruit;

LedArray::LedArray(
	Platform::Spi& spi, Platform::Gpio& LAT, Platform::Gpio& OE,
	Platform::Gpio& A, Platform::Gpio& B, Platform::Gpio& C, Platform::Gpio& D
):
    Adafruit_GFX(32, 32),
    m_spi(spi), m_LAT(LAT), m_OE(OE), m_A(A), m_B(B), m_C(C), m_D(D)
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

void LedArray::activateFrame()
{
    // Specific
    disableOutput();
    // Specific
    latch();
    // Specific
    updateScanLine();
    // Specific
    enableOutput();
}

void LedArray::disableOutput()
{
    // I am assuming OE polarity is active low
    m_OE=true;
}

void LedArray::enableOutput()
{
    m_OE=!m_active;
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

void LedArray::transferNextFrame()
{
    // initiate the DMA transfer now
    if(m_idle && m_current_pixel_weight==(BIT_PER_PIXEL-1)) return;
    m_spi.send(m_datalines[m_current_pixel_weight+(m_idle?1:0)][m_current_scanline], BYTES_PER_SCANLINE);
}

void LedArray::setOutputGPIO(Platform::Gpio& pin)
{
    pin
	.setOutput()
	.setSpeed(Platform::Gpio::SPEED_100MHz)
	.setPushPull();
}

void LedArray::blank()
{
    fill(0x000000);
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
    // Some GFX libraries tend to draw off the bitmap. We truncate here.
    if(line>=SCANLINES*2) return;
    if(column>=32) return;
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

void LedArray::enterSleepMode()
{
    PseudoPWMDisplay::enterSleepMode();
    disableOutput();
}

// Demote 8/8/8 to Adafruit_GFX 5/6/5
// If no gamma flag passed, assume linear color
uint16_t LedArray::Color888(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

uint16_t LedArray::Color24(uint32_t rgb)
{
    return Color888((rgb>>16) & 0xFF, (rgb>>8) & 0xFF, rgb & 0xFF);
}

uint32_t fromColor565(uint16_t c)
{
// R: bits 15 to 11 (inc) moved to bits 23 to 16 with 3 bits of padding
// G: bits 10 to 5 (inc) moved to bits 15 to 8 with 2 bits of padding
// B: bits 4 to 0 (inc) moved to bits 7 to 0 with 3 bits of padding
    return ((c & 0xF800) << 8) | ((c & 0x07E0) << 5) | ((c & 0x1F) << 3);
}

// Adafruit GFX compatibility layer
void LedArray::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    setPixelAt(fromColor565(color), y, x);
}

