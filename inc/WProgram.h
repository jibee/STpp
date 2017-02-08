#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef bool boolean;

class Print
{
    public:
	size_t print(const char[]);
	size_t print(char);
#if ARDUINO >= 100
	virtual size_t write(uint8_t) = 0;
#else
	virtual void   write(uint8_t) = 0;
#endif

};

class __FlashStringHelper
{
};
