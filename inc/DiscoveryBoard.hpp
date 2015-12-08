#include <STM32F4.hpp>

using namespace Platform;

class DiscoveryBoard: public Platform::STM32F4_WithUSB
{
public:
DiscoveryBoard();
    Gpio g_LedG;
    Gpio g_LedO;
    Gpio g_LedR;
    Gpio g_LedB;

};