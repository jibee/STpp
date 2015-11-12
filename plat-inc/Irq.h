#ifndef _IRQ_H
#define _IRQ_H

namespace Platform
{
/** CPU nested vectored interruptions management
 * @todo find out what it entails? If that just a control part and the
 * actual interrupt vectors are left untouched?
 */
class Irq {
    private:
	int irq;
    public:
	/** Constructor
	  @param n Interruption number
	 */
	Irq(int n);
	/** Enable the interruption source */
	Irq& enable();
	/** Disable the interruption source */
	Irq& disable();
	/** Sets the interruption priority */
	Irq& setPriority(int ip);
};
}
#endif /* IRQ_H */
