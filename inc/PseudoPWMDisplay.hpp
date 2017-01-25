#ifndef PSEUDOPWMDISPLAY_HPP
#define PSEUDOPWMDISPLAY_HPP

#include <Timer.h>
#include <IPowerSavingDevice.hpp>
class PseudoPWMDisplay: public IPowerSavingDevice
{
    public:
	PseudoPWMDisplay();
	virtual ~PseudoPWMDisplay();
	// Timer interrupt handler; called every 0.5ms to possibly update the output data
	void tick();
	void setTimer(Platform::Timer& hwTimer);
	virtual void enterActiveMode();
	virtual void enterSleepMode();
	virtual void enterIdleMode();
    protected:
	static const int BIT_PER_PIXEL = 8;
	static const int SCANLINES = 16;
	static const int SKIP_LOW_WEIGHT_BITS = 4;
	// Moves to the next scanline
	void shiftNextScanline();
	// First dimension into the m_datalines
	int m_current_pixel_weight;
	// Second dimension into the scanlines
	int m_current_scanline;
	// current position in the PWM timing; counting downward;
	int m_current_pseudo_pwm_counter;


	virtual void activateFrame()=0;
	// initiates the transfer of the current scanline
	virtual void transferNextFrame()=0;
	enum Component { Red, Green, Blue };
	static bool colorHasComponent(uint32_t color, Component component, int weight);
	bool m_active;
	bool m_idle;
};

#endif /* PSEUDOPWMDISPLAY_HPP */
