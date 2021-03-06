#ifndef _TIMER_H
#define _TIMER_H
#include <tr1/functional>
#include <Gpio.h>
#include <stm32f4xx.h>

#ifndef __attribute_deprecated__
#define __attribute_deprecated__ __attribute((deprecated))
#endif

class Shell;

namespace Platform
{
/** hardware timer / counter
 *
 * @todo certain timers do not support all features; to reflect this I would suggest
 * to templatise this class and add some form of hierarchy.
 *
 * * TIM1 and 8: Advanced control timers with 16 bit counters and 4 channels and complementary outputs
 * * TIM2 and 5: General Purpose timer with 32 bit counters and 4 channels
 * * TIM3 and 4: General Purpose timer with 16 bit counters and 4 channels
 * * TIM9 and 12: General Purpose timer with 16 bit counters and 2 channels
 * * TIM10, 11, 13 and 14: General Purpose timer with 16 bit counters and 1 channel
 * * TIM 6 and 7: basic timer
 */
class Timer
{
    protected:
	/** base registers for the timer configuration */
	volatile TIM_TypeDef* base;
	/** Timer number */
	int number;
    public:
	/** Constructor.
	 *
	 * @param b timer control register base address
	 * @param n timer number.
	 *
	 * @todo presumably the base register depends on the timer number. We should be
	 * able to internalise the base register determination rather than rely on the
	 * caller goodwill.
	 *
	 * */
	Timer(volatile TIM_TypeDef* b, int n);
	//All timers
	/** @name timer operations
	 *
	 * These operations are valid for all timers
	 * */
	///@{
	/** Set the timer prescaler.
	 *
	 * The timer clock is first scaled (down) by this factor.
	 *
	 * This is a 16 bit integer.
	 *
	 * @param prescaler divisor value for the timer
	 *
	 * */
	Timer& setPrescaler(uint16_t prescaler);
	/** Set the timer count down value for reset at next event
	 *
	 * @param reloadValue reload value for the timer
	 * */
	Timer& setAutoReload(uint16_t reloadValue);
	/** Obtains the automatic reload value
	 *
	 */
	unsigned short getAutoReload();
	/** Enable buffering of the AutoReload register
	 *
	 * @param enable when set to false, the counter limit is updated immediately -
	 * including the current timer cycle.
	 * When set to true the counter limit is updated only upon the next underflow
	 * or overflow.
	 */
	Timer& setAutoReloadBuffered(bool enable);
	/** Set the value of the counter
	 *
	 * @param value value of the counter
	 */
	Timer& setCounter(uint16_t value);
	/** Actively wait for the timer to elapse.
	 *
	 * */
	Timer& wait();
	/** Obtains the value of the counter
	 *
	 * */
	unsigned short getCounter();
	/** Sets the one-pulse mode
	 *
	 * @param enable set to true to stop the counter at the next event (clearing bit CEN).
	 * Set to false to leave the counter running after the next event.
	 * */
	Timer& setOneShot(bool enable);
	/**
	 * Enable/disable UEV event generation.
	 *
	 * The Update (UEV) event is generated by one of the following events:
	 * - Counter overflow/underflow
	 * - Setting the UG bit
	 * - Update generation through the slave mode controller
	 *
	 * @param disable when set to false, UEV is enabled. When the UEV event is generated,
	 * buffered registers are loaded with their preload values.
	 * When set to true, the Update event is not generated, shadow registers keep their value
	 * (ARR, PSC, CCRx). However the counter and the prescaler are reinitialized if the UG bit is
	 * set or if a hardware reset is received from the slave mode controller.
	 * */
	Timer& setUpdateDisable(bool disable);
	/** Enables the timer.
	 *
	 * From this point onwards the counters will be incremented or decremented
	 *
	 * */
	Timer& enable();
	/** Disables the timer
	 *
	 * */
	Timer& disable();
	/**
	 * Obtains the running status of the timer, including whether the timer elapsed
	 * */
	bool enabled();
	/** Obtains the timer number
	 *
	 * */
	int getNumber();
	/// @}
	//Not all timers
	/** Channel input/output configuration
	 * */
	enum Direction {
	    /** Configure the channel as output */
	    OUTPUT,
	    /** Configures as input, maps the channel on the direct timer input (ie 1 for channel 1, 2 for channel 2 etc) as relevant */
	    INPUT1,
	    /** Configures as input, maps the channel on the associated direct timer input (ie 2 for channel 1, 1 for channel 2 etc) as relevant */
	    INPUT2,
	    /** Configures as input using internal trigger */
	    INPUT3
	};
	// All general purpose timers
	/** Set the capture/compare channel direction
	 *
	 * @param channel channel number - from 1 to 4
	 * @param direction capture channel configuration and direction
	 */
	Timer& setChannelDirection(int channel, Direction direction);

	/** @todo not implemented */
	Timer& setChannelCompareBuffered(int,bool);

	/**
	 * Compare output modes
	 * @todo add the missing 4 values - Frozen; Toggle; Force inactive; Force active
	 *
	 * */
	enum ChannelMode {
	    /**
	     * Set the channel to active level on match.
	     *
	     * OC1REF signal is forced high when the counter TIMx_CNT matches the
	     * capture/compare register 1 (TIMx_CCR1).
	     * */
	    MatchHigh,
	    /**
	     * Set the channel to inactive level on match.
	     *
	     * OC1REF signal is forced low when the counter TIMx_CNT matches the
	     * capture/compare register 1 (TIMx_CCR1).
	     * */
	    MatchLow,
	    /**
	     * PWM mode 1.
	     * In upcounting, the channel is active as long as TIMx_CNT&lt;TIMx_CCR1 else inactive.
	     * In downcounting, the channel is inactive as long as TIMx_CNT&gt;TIMx_CCR1 else active.
	     * */
	    CompareHigh,
	    /**
	     * PWM mode 2.
	     * In upcounting, the channel is inactive as long as TIMx_CNT&lt;TIMx_CCR1 else active.
	     * In downcounting, the channel is active as long as TIMx_CNT&gt;TIMx_CCR1 else inactive.
	     * */
	    CompareLow
	};
	// All general purpose timers
	/** Set the channel output mode.
	 *
	 * Note for this to work you need to set the comparator value
	 *
	 * @param channel one of the 4 channels - 1 to 4
	 * @param mode a ChannelMode compare and output mode
	 * @todo replace the pointer arithmetics with proper register operations
	 * @todo replace the channel number with an enum value
	 * */
	Timer& setChannelMode(int channel, ChannelMode mode);
	/** Enable the channel output
	 *
	 * @param channel one of the 4 channels - 1 to 4
	 * @param enable if the channel is configured as output, enables output of the
	 * comparator; when false the the signal is left inactive. When the channel is
	 * configured as an input, this enables the capture of external trigger events
	 *
	 */
	Timer& setChannelOutput(int channel, bool enable);
	/** Force the emission of an UEV Update event. This loads the registers atomically to their preloaded values
	 *
	 * */
	Timer& update();
	/** Set the capture and compare 1 selection.
	 *
	 * @deprecated use setChannelDirection instead
	 * */
	__attribute_deprecated__ Timer& setCaptureCompare1Sel(int);
	/** Set the capture and compare 2 selection.
	 *
	 * @deprecated use setChannelDirection instead
	 * */
	__attribute_deprecated__ Timer& setCaptureCompare2Sel(int);

	/**
	 * Set the capture/compare polarity for channel 1
	 *
	 * @param in set to true if the Timer is in input mode
	 * @param v value to load in the Compare polarity register:
	 *
	 * When configured as input, selects the polarity for trigger or capture operations.
	 * 00: noninverted/rising edge: Circuit is sensitive to TIxFP1 rising edge (capture,
	 * trigger in reset, external clock or trigger mode), TIxFP1 is not inverted
	 * (trigger in gated mode, encoder mode).
	 * 01: inverted/falling edge
	 * Circuit is sensitive to TIxFP1 falling edge (capture, trigger in reset, external
	 * clock or trigger mode), TIxFP1 is inverted (trigger in gated mode, encoder mode).
	 *
	 * 10: reserved, do not use this configuration.
	 *
	 * 11: noninverted/both edges
	 * Circuit is sensitive to both TIxFP1 rising and falling edges (capture, trigger in
	 * reset, external clock or trigger mode), TIxFP1 is not inverted (trigger in gated
	 * mode). This configuration must not be used for encoder mode.
	 *
	 * When configured as output:
	 * 0: OC1 active high
	 * 1: OC1 active low
	 *
	 * @todo make a higher level version of this function
	 * @todo determine if we are in input mode from the current state
	 *
	 * */
	Timer& setCaptureCompare1Polarity(bool in, int v);
	/**
	 * Set the capture/compare polarity for channel 1
	 *
	 * @param in set to true if the Timer is in input mode
	 * @param v value to load in the Compare polarity register:
	 *
	 * When configured as input, selects the polarity for trigger or capture operations.
	 * 00: noninverted/rising edge: Circuit is sensitive to TIxFP1 rising edge (capture,
	 * trigger in reset, external clock or trigger mode), TIxFP1 is not inverted
	 * (trigger in gated mode, encoder mode).
	 * 01: inverted/falling edge
	 * Circuit is sensitive to TIxFP1 falling edge (capture, trigger in reset, external
	 * clock or trigger mode), TIxFP1 is inverted (trigger in gated mode, encoder mode).
	 *
	 * 10: reserved, do not use this configuration.
	 *
	 * 11: noninverted/both edges
	 * Circuit is sensitive to both TIxFP1 rising and falling edges (capture, trigger in
	 * reset, external clock or trigger mode), TIxFP1 is not inverted (trigger in gated
	 * mode). This configuration must not be used for encoder mode.
	 *
	 * When configured as output:
	 * 0: OC1 active high
	 * 1: OC1 active low
	 *
	 * @todo make a higher level version of this function
	 * @todo determine if we are in input mode from the current state
	 *
	 * */
	Timer& setCaptureCompare2Polarity(bool in, int v);
	/** Define the frequency used to sample TI1 input and the length of the digital filter
	 * applied to TI1. The digital filter is made of an event counter in which N events
	 * are needed to validate a transition on the output:
	 *
	 * 0000: No filter, sampling is done at fDTS
	 * 0001: fSAMPLING=fCK_INT , N=2
	 * 0010: fSAMPLING=fCK_INT , N=4
	 * 0011: fSAMPLING=fCK_INT , N=8
	 * 0100: fSAMPLING=fDTS/2, N=6
	 * 0101: fSAMPLING=fDTS/2, N=8
	 * 0110: fSAMPLING=fDTS/4, N=6
	 * 0111: fSAMPLING=fDTS/4, N=8
	 * 1000: fSAMPLING=fDTS/8, N=6
	 * 1001: fSAMPLING=fDTS/8, N=8
	 * 1010: fSAMPLING=fDTS/16, N=5
	 * 1011: fSAMPLING=fDTS/16, N=6
	 * 1100: fSAMPLING=fDTS/16, N=8
	 * 1101: fSAMPLING=fDTS/32, N=5
	 * 1110: fSAMPLING=fDTS/32, N=6
	 * 1111: fSAMPLING=fDTS/32, N=8
	 *
	 * Note:In current silicon revision, fDTS is replaced in the formula by CK_INT when ICxF[3:0]= 1, 2 or 3.
	 * */
	Timer& setInputCapture1Filter(int);
	/** Define the frequency used to sample TI2 input and the length of the digital filter
	 * applied to TI2. The digital filter is made of an event counter in which N events
	 * are needed to validate a transition on the output:
	 *
	 * 0000: No filter, sampling is done at fDTS
	 * 0001: fSAMPLING=fCK_INT , N=2
	 * 0010: fSAMPLING=fCK_INT , N=4
	 * 0011: fSAMPLING=fCK_INT , N=8
	 * 0100: fSAMPLING=fDTS/2, N=6
	 * 0101: fSAMPLING=fDTS/2, N=8
	 * 0110: fSAMPLING=fDTS/4, N=6
	 * 0111: fSAMPLING=fDTS/4, N=8
	 * 1000: fSAMPLING=fDTS/8, N=6
	 * 1001: fSAMPLING=fDTS/8, N=8
	 * 1010: fSAMPLING=fDTS/16, N=5
	 * 1011: fSAMPLING=fDTS/16, N=6
	 * 1100: fSAMPLING=fDTS/16, N=8
	 * 1101: fSAMPLING=fDTS/32, N=5
	 * 1110: fSAMPLING=fDTS/32, N=6
	 * 1111: fSAMPLING=fDTS/32, N=8
	 *
	 * Note:In current silicon revision, fDTS is replaced in the formula by CK_INT when ICxF[3:0]= 1, 2 or 3.
	 * */
	Timer& setInputCapture2Filter(int);
	/**
	 * Slave mode selection.
	 *
	 * @param mode Trigger mode selection
	 *
	 * When external signals are selected, the active edge of the trigger signal (TRGI)
	 * is linked tothe polarity selected on the external input (see Input control
	 * register and Control register descriptions.
	 *
	 * 000: Slave mode disabled - if CEN = 1 then the prescaler is clocked directly by
	 * the internal clock
	 * 001: Encoder mode 1 - Counter counts up/down on TI2FP2 edge depending on TI1FP1
	 * level.
	 * 010: Encoder mode 2 - Counter counts up/down on TI1FP1 edge depending on TI2FP2
	 * level.
	 * 011: Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges
	 * depending on the level of the other input.
	 * 100: Reset mode - Rising edge of the selected trigger input (TRGI) reinitializes
	 * the counter and generates an update of the registers
	 * 101: Gated mode - The counter clock is enabled when the trigger input (TRGI) is
	 * high. The counter stops (but is not reset) as soon as the trigger becomes low.
	 * Counter starts and stops are both controlled
	 * 110: Trigger mode - The counter starts on a rising edge of the trigger TRGI (but
	 * it is not reset). Only the start of the counter is controlled
	 * 111: External clock mode 1 - Rising edges of the selected trigger (TRGI) clock the
	 * counter
	 *
	 * Note that 001, 010 and 011 are only valid for timers 1 to 5 and 8
	 */
	Timer& setSlaveModeSelection(int mode);
	/** Indicates if the timer is active
	 * */
	operator bool();

	//Interrupt handling
    public:
	/** Interrupt handler callback
	 * */
	typedef std::tr1::function<void(int)> Callback;
	/** Obtains the IRQ number for interruptions triggered by this Timer.
	 *
	 * Original code states this is the most interesting function ever. I
	 * strongly disaggree (but that's because I write too much Java these days)
	 * */
	int irqNr();
	/**
	 * Enable raising an interrupt on an Update signal
	 *
	 * @param enable set to true to receive interrupts on Update events
	 * */
	Timer& setUIE(bool);
	/**
	 * Update request source
	 *
	 * @param onlyOverflow send update events only on overflow events
	 *
	 * This bit is set and cleared by software to select the UEV event sources.
	 *
	 * false: Any of the following events generate an update interrupt or DMA request if enabled.
	 * These events can be:
	 * - Counter overflow/underflow
	 * - Setting the UG bit
	 * - Update generation through the slave mode controller
	 *
	 * true: Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
	 */
	Timer& setURS(bool);
	/** Entry point for Interrupt handlers
	 *
	 * This in turn calls the callback set up by setTopCB
	 * */
	static void callTopCb(int nr);
	/** Sets the function to be called upon the triggering of an update event
	 *
	 * @param cb the function to call upon such event
	 * */
	Timer& setTopCB(Callback cb);

    private:
	/** Supposedly clears the Interrupt flag after handling. No-op in practice */
	Timer& clearInterrupt();
};


template <typename COUNTER_SIZE, int channels>
class GeneralPurposeTimer: public Timer
{
    public:
	GeneralPurposeTimer(volatile TIM_TypeDef* b, int n):
	    Timer(b, n)
	{
	}
	/**
	 * Set the AF function of a GPIO if it is used with this timer
	 *
	 * */
	GeneralPurposeTimer<COUNTER_SIZE, channels>& setAlternate(Gpio& gpio)
	{
	    if (number <= 2)
		gpio.setAlternate(Gpio::TIM1_2);
	    else if (number <= 5)
		gpio.setAlternate(Gpio::TIM3_5);
	    else if (number <= 7)
		// There are no PWM output for TIM6 and TIM7
		for(;;);
	    else if (number <= 11)
		gpio.setAlternate(Gpio::TIM8_11);
	    else if (number <= 14)
		gpio.setAlternate(Gpio::CAN1_2_TIM12_14);
	    else
		for(;;);
	    return *this;
	}
	/** Sets the value of the channel comparator
	 *
	 * @param channel one of the 4 channels - 1 to 4
	 * @param referenceValue value to be set for the reference value
	 * Note that when setAutoReloadBuffered is acive the comparator value will
	 * be reset to the AutoReload value upon next event.
	 *
	 * @todo Timer 2 and 5: referenceValue is a uint32_t
	 * */
	GeneralPurposeTimer<COUNTER_SIZE, channels>& setChannelComparator(int channel, COUNTER_SIZE referenceValue){
	    channel--;
	    volatile unsigned int *b=(volatile unsigned int*)&(base->CCR1);
	    b+=channel;
	    *b=referenceValue;
	    return *this;
	}

	/** Obtains the value of the channel comparator
	 *
	 * @param channel one of the 4 channels - 1 to 4
	 * @return when configured as an input, this is the value of the counter
	 * as of the latest trigger event. When configured as an output, this is the
	 * value of the comparator as set by setChannelComparator
	 *
	 * @todo Timer 2 and 5: return a uint32_t
	 * */
	COUNTER_SIZE getChannelComparator(int channel)
	{
	    channel--;
	    volatile unsigned int *b=(volatile unsigned int*)&(base->CCR1);
	    b+=channel;
	    return *b;
	}


};

class AdvancedControlTimer: public GeneralPurposeTimer<uint16_t, 4>
{
    public:
	AdvancedControlTimer(volatile TIM_TypeDef* b, int n): GeneralPurposeTimer< uint16_t, 4 >(b,n)
	{
	}
};

}

Shell& operator<<(Shell& s, Platform::Timer& tim);


#endif /* _TIMER_H */
