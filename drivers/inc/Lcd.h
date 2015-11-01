#ifndef LCD_H
#define LCD_H
#include <Gpio.h>
#include <Lock.h>
#include <OStream.h>

class Lcd;
class LcdLine : public OStream {
	private:
		Lcd* lcd;
		int position;
		int line;
		LcdLine();
		void set(Lcd&, int);
		friend class Lcd;
		bool locked;
	public:
		virtual LcdLine& put(char);
		virtual LcdLine& endl();
};

class Lcd {
	//Private methods are not locked
	//Publics ones are.
	private:
		friend class LcdLine;
		void write(int r, int val);
		void write4(int r, int val);
		void init();
		void wait(bool quick);
		Platform::Gpio RS, E, DB7, DB6, DB5, DB4;
		bool is_4bit;
		Platform::Mutex lock;
	public:
		Lcd(Platform::Gpio RS, Platform::Gpio E, Platform::Gpio DB7, Platform::Gpio DB6, Platform::Gpio DB5, Platform::Gpio DB4);
		// 0 0 0 0 0 0 0 1
		Lcd& clearDisplay();
		// 0 0 0 0 0 0 1 *
		Lcd& returnHome();
		// 0 0 0 0 0 1 I D
		Lcd& setDirection(bool increment, bool display);
		// 0 0 0 0 1 O C B
		Lcd& displaySet(bool on, bool cursor, bool blink);
		// 0 0 0 1 x x * *
		Lcd& move(bool cursor, bool left);
	private:
		// 0 0 1 8 N F * *
		Lcd& functionSet(bool is_8bits, bool is_2lines, bool font);
		//0 1 A5 A4 A3 A2 A1 A0
		Lcd& setCGRAM(int addr);
		//1 A6 A5 A4 A3 A2 A1 A0
		Lcd& setDDRAM(int addr);
		
		Lcd& put(char c);
		Lcd& put(const char* str);
	public:
		LcdLine& operator()(int);
		LcdLine first;
		LcdLine second;
};

#endif
