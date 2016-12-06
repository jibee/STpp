#include <PseudoPWMDisplay.hpp>
#include <Irq.h>

PseudoPWMDisplay::PseudoPWMDisplay():
    m_current_pixel_weight(0),
    m_current_scanline(0),
    m_current_pseudo_pwm_counter(0)
{
}

PseudoPWMDisplay::~PseudoPWMDisplay()
{
}

void PseudoPWMDisplay::tick()
{
    m_current_pseudo_pwm_counter--;
    if(m_current_pseudo_pwm_counter<=0)
    {
	m_current_pseudo_pwm_counter = 1<<(m_current_pixel_weight-SKIP_LOW_WEIGHT_BITS);
	activateFrame();
	// Generic
	shiftNextScanline();
	// Specific
	transferNextFrame();
    }
}
void PseudoPWMDisplay::shiftNextScanline()
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
}

void PseudoPWMDisplay::setTimer(Platform::Timer& hwTimer)
{
    hwTimer
    // We need to be interrupted 102 400 times per second (25 fps, 256 PWM levels, 16 scanlines)
	.setPrescaler(1<<SKIP_LOW_WEIGHT_BITS)
	.setAutoReload(469)
	.setCounter(469)
	.setUIE(true)
	.setURS(true)
	.setOneShot(false)
	.setTopCB([this](int){this->tick();});
    Platform::Irq(hwTimer.irqNr()).setPriority(15).enable();
    hwTimer
	.enable();
}

