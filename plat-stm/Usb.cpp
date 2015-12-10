#include <stm32f4xx.h>
#include <Board.h>
#include <Irq.h>
#include <Usb.h>
#include <Log.h>
#include <Exti.h>
#include <STM32F4.hpp>

extern "C" {
#include <usb_defines.h>
#include <usb_core.h>
#include <usbd_core.h>
#include <usbd_hid_core.h>
#include <usbd_msc_core.h>
#include <usbd_cdc_core.h>
};

extern "C" {
	USB_OTG_CORE_HANDLE  USB_OTG_dev;
	extern USBD_DEVICE USR_desc;
	void USB_OTG_BSP_Init(void);
	void USB_OTG_BSP_EnableInterrupt(void);
	void USB_OTG_BSP_DriveVBUS(uint32_t speed, uint8_t state);
	void USB_OTG_BSP_ConfigVBUS(uint32_t speed);
	void USB_OTG_BSP_uDelay (const uint32_t usec);
	void USB_OTG_BSP_mDelay (const uint32_t msec);
	void USB_OTG_BSP_TimerIRQ (void);

	extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
	void OTG_FS_IRQHandler(void) {
		USBD_OTG_ISR_Handler (&USB_OTG_dev);
	}
};

using namespace Platform;

Gpio* vbus_en;
Timer* uTimer;

void USB_OTG_BSP_Init(void) {
	RCC->AHB2ENR |= 1<<7;
}

void USB_OTG_BSP_EnableInterrupt(void) {
	Irq(OTG_FS_IRQn)
		.setPriority(254)
		.enable();
}

void USB_OTG_BSP_DriveVBUS(uint32_t speed, uint8_t state) {
	(void)speed;
	vbus_en->setState(state == 0);
}

void USB_OTG_BSP_ConfigVBUS(uint32_t speed) {
	(void)speed;

}

void USB_OTG_BSP_uDelay (const uint32_t usec)
{
#if 1
    uTimer->setAutoReload(usec*42);
	uTimer->enable();
	uTimer->wait();
	uTimer->disable();
#else
  uint32_t count = 0;
  const uint32_t utime = (120 * usec / 7);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1); 
#endif
}

void USB_OTG_BSP_mDelay (uint32_t msec)
{
	while(msec) {
		USB_OTG_BSP_uDelay(1000);
		msec--;
	}
}

void USB_OTG_BSP_TimerIRQ (void)
{

} 


void USBD_USR_Init() {
	Log::log << "USBD initing" << endl;
}

void USBD_USR_DeviceReset(uint8_t speed ) {
	(void)speed;
	Log::log << "USBD resting" << endl;
}

void USBD_USR_DeviceConfigured (void) {
	Log::log << "HID interface started" << endl;
}

void USBD_USR_DeviceConnected (void) {
	Log::log << "HID interface starting" << endl;
}

void USBD_USR_DeviceDisconnected (void) {
	Log::log << "HID interface stopped" << endl;
}

void USBD_USR_DeviceSuspended() {
	Log::log << "USB suspend" << endl;
}

void USBD_USR_DeviceResumed() {
	Log::log << "Device woke up" << endl;
}

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,  
};

static bool occupied;
Usb::Usb(STM32F4_WithUSB& board) {
	if(occupied)
		while(1);

	occupied = true;
    // TODO perhaps specify this externally too?
    uTimer = &(board.Tim6);
	uTimer->setPrescaler(1)
		.setAutoReloadBuffered(true)
		.setOneShot(true);

	board.USB_Vbus_en
		.setSpeed(Gpio::SPEED_100MHz)
		.setPushPull()
		.setFunction(Gpio::GPIO)
		.setDirection(Gpio::OUTPUT)
		//Enforce vbus off
		.setState(true);
    vbus_en = &(board.USB_Vbus_en);
	board.USB_DM
		.setSpeed(Gpio::SPEED_100MHz)
		.setAlternate(Gpio::OTG_FS_HS);
	board.USB_DP
		.setSpeed(Gpio::SPEED_100MHz)
		.setAlternate(Gpio::OTG_FS_HS);

	board.USB_ID
		.setOpenDrain()
		.setFunction(Gpio::ALTERNATE)
		.setDirection(Gpio::OUTPUT)
		.setResistor(Gpio::PULL_UP)
		.setAlternate(Gpio::OTG_FS_HS);

	if(board.USB_Vbus_det)
		Log::log << "Found vbus, we're (probably) guest" << endl;
	else if(!board.USB_ID) {
		Log::log << "Found a USB OTG A, we're host" << endl;
		board.USB_Vbus_en = false;
		Log::log << "Enabled vbus" << endl;
	} else {
		Log::log << "We are currently disconnected..." << endl;
	}
}
