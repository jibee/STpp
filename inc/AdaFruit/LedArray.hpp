/**
 * AdaFruit Led Array driver
 * 
 * This class drives a Led Array as found on from AdaFuit.
 * 
 * The Geometry of the array is specified by template arguments.
 * 
 * This driver requires a non-standard wiring to offload most of the
 * communications to the STM32 hardware:
 * R1 must be connected to a SPI MOSI pin
 * G1 must be connected to the chaining output of the array R1
 * B1 must be connected to the chaining output of the array G1
 * R2 must be connected to the chaining output of the array B1
 * G2 must be connected to the chaining output of the array R2
 * B2 must be connected to the chaining output of the array G2
 * CLK must be connected to the SPI CLK pin
 * A, B, C, D, OE and LAT must be connected to standard GPIOs
 * */


#ifndef DEVICES_ADAFRUIT_LEDARRAY_H
#define DEVICES_ADAFRUIT_LEDARRAY_H

#include <Spi.h>
#include <PseudoPWMDisplay.hpp>
#include "Adafruit_GFX.h"

namespace AdaFruit
{
    class LedArray: public PseudoPWMDisplay, Adafruit_GFX
    {
	public:
	    LedArray(
		    Platform::Spi& spi, Platform::Gpio& LAT, Platform::Gpio& OE,
		    Platform::Gpio& A, Platform::Gpio& B, Platform::Gpio& C, Platform::Gpio& D
		    );
	    void setPixelAt(uint32_t color, uint8_t line, uint8_t column);
	    void blank();
	    // Fill the whole screen with the given color
	    void fill(uint32_t color);
	    void verticalLine(uint32_t color, uint8_t column);
	    // Draws an horizontal line
	    void horizontalLine(uint32_t color, uint8_t line);
	    virtual void enterSleepMode();
	    // Adafruit GFX compatibility layer
	    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
	private:
	    Platform::Spi& m_spi;
	    Platform::Gpio& m_LAT;
	    Platform::Gpio& m_OE;
	    Platform::Gpio& m_A;
	    Platform::Gpio& m_B;
	    Platform::Gpio& m_C;
	    Platform::Gpio& m_D;
	    static const int BYTES_PER_SCANLINE = 24;

	    char m_datalines[BIT_PER_PIXEL][SCANLINES][BYTES_PER_SCANLINE] ;

	    virtual void activateFrame();
	    // Switches off the led output
	    void disableOutput();
	    // Switches on the led output
	    void enableOutput();
	    // Latches the shift register data into the output
	    void latch();
	    // Updates the scanline bits
	    void updateScanLine();
	    // initiates the transfer of the current scanline
	    virtual void transferNextFrame();
	    // Configures the GPIO pin
	    void setOutputGPIO(Platform::Gpio& pin);
    };    
};



#endif
