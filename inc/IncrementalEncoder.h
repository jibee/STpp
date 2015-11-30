#ifndef _INCREMENTAL_ENCODER_H
#define _INCREMENTAL_ENCODER_H
#include <Gpio.h>
#include <Timer.h>


// #TODO add support for the zero pin of the encoder
class IncrementalEncoder {
    private:
	Platform::Gpio& encoderA;
	Platform::Gpio& encoderB;

    protected:
	IncrementalEncoder(Platform::Gpio& a, Platform::Gpio& b, bool reverse = false);
	int counter;
	int last_value;
	bool reverse;
    public:
	//Must be called often enough (less than half a timer cycle has been made since last update)
	virtual IncrementalEncoder& update()=0;
	int getCounter();
	operator int();
	int operator=(int);
	void setValue(int v);
};

template<typename COUNTER_SIZE>
class _IncrementalEncoder_impl: public IncrementalEncoder
{
    private:
	Platform::GeneralPurposeTimer<COUNTER_SIZE, 4>& tim;

    protected:
	_IncrementalEncoder_impl(
		Platform::Gpio& a,
		Platform::Gpio& b,
		Platform::GeneralPurposeTimer<COUNTER_SIZE, 4>& t,
		int prescaler=0,
		bool reverse=false
		);
	//Must be called often enough (less than half a timer cycle has been made since last update)
	virtual _IncrementalEncoder_impl<COUNTER_SIZE>& update();
};

class IncrementalEncoder32: public _IncrementalEncoder_impl< uint32_t >
{
    public:
	IncrementalEncoder32(
		Platform::Gpio& a,
		Platform::Gpio& b,
		Platform::GeneralPurposeTimer< uint32_t, int(4) >& t,
		int prescaler = 0,
		bool reverse = false
		):
	    _IncrementalEncoder_impl< uint32_t >(a,b,t,prescaler,reverse)
    {
    }
};

class IncrementalEncoder16: public _IncrementalEncoder_impl< uint16_t >
{
    public:
	IncrementalEncoder16(
		Platform::Gpio& a,
		Platform::Gpio& b,
		Platform::GeneralPurposeTimer< uint16_t, int(4) >& t,
		int prescaler = 0,
		bool reverse = false
		):
	    _IncrementalEncoder_impl< uint16_t >(a,b,t,prescaler,reverse)
    {
    }
};

/**
 * @brief Creates an IncrementalEncoder object
 *
 * @params a           Gpio of the A pin of the Incremental Encoder
 * @params b           Gpio of the B pin of the Incremental Encoder
 * @params t           Timer on which the encoder's gpios are plugged
 * @params prescaler   Timer prescaler
 * @params reverse     Set to 1 if the pins A and B are switched (as the timer has no way of knowing which one is A or B)
 *
 */
template <typename COUNTER_SIZE>
_IncrementalEncoder_impl<COUNTER_SIZE>::_IncrementalEncoder_impl(
	Platform::Gpio& a,
	Platform::Gpio& b,
	Platform::GeneralPurposeTimer<COUNTER_SIZE, 4>& t,
	int prescaler,
	bool reverse
	):
    IncrementalEncoder(a, b, reverse), tim(t)
{

    tim.setAlternate(a);
    tim.setAlternate(b);

    tim
	.setPrescaler(prescaler)
	.setAutoReload(0xffff)
	.setCaptureCompare1Sel(1)
	.setCaptureCompare2Sel(1)
	.setCaptureCompare1Polarity(1,0)
	.setCaptureCompare2Polarity(1,0)
	.setInputCapture1Filter(0)
	.setInputCapture2Filter(0)
	.setSlaveModeSelection(3)
	.enable();
}

/**
 * @brief Computes the absolute coder's position
 *
 * @details assuming the previous position is less than half a timer's cycle away
 *
 */
template <typename COUNTER_SIZE>
_IncrementalEncoder_impl<COUNTER_SIZE>& _IncrementalEncoder_impl<COUNTER_SIZE>::update()
{
    int v = tim.getCounter();

    // TODO we assume that the counter is 16 bits here. Generalise using limits<COUNTER_SIZE>
    // Assume we're going forward
    int d = v - last_value;
    if(d <= 0)
	d+=65536;


    if(d >= 32768) {
	// We're too far, we were wrong
	// Meaning we're actally going backward
	d = d - 65536;
    }
    counter += d;

    last_value = v;
    return *this;
}

#endif /* _INCREMENTAL_ENCODER_H */
