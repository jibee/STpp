#include <DiscoveryBoard.hpp>

DiscoveryBoard::DiscoveryBoard(): STM32F4_WithUSB(),
g_LedG(GpioD[12]),
g_LedO(GpioD[13]),
g_LedR(GpioD[14]),
g_LedB(GpioD[15])
{
}