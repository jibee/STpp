#ifndef _TASKS_H
#define _TASKS_H

extern "C" {
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
};
#include <tr1/functional>
namespace RTOS
{
/** RTOS task 
 * */
class Task {
	public:
        /** Runnable function signature (takes no argument, returns nothing)
         * 
         * If you want to pass an argument you can use bind() to create a lambda 
         * and conform it to this signature
         */
		typedef std::tr1::function<void(void)> Function;
        /** Handle to the function to be called when the task runs */
		Function f;
        /** RTOS Task handle */
		xTaskHandle _id;
        /** Constructor 
         * 
         * @param f function to be executed by the task
         * @param name name of the task
         * @param priority pritority level of the task
         * @param stackSize size of the thread stack
         * 
         * */
		Task(Function f, const char *name, int priority = tskIDLE_PRIORITY+2, int stackSize = 256);
        /** Suspend the execution of the task. Call resume to resume the task */
		void suspend();
        /** Resume the execution of the task after it was suspended with suspend */
		void resume();
        /** Voluntarily yield the control of the active task to the next thread */
		static void yield();
};
}

#endif /* _TASKS_H */
