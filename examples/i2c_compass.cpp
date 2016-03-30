#include <I2C.hpp>
#include <DiscoveryBoard.hpp>
#include <Time.h>
#include <Gpio.h>
#include <Pwm.h>

class LedAngleDisplay
{
public:
    LedAngleDisplay(DiscoveryBoard& board):
    LedG(board.g_LedG, board.Tim4, 1),
    LedO(board.g_LedO, board.Tim4, 2),
    LedR(board.g_LedR, board.Tim4, 3),
    LedB(board.g_LedB, board.Tim4, 4)
    {
    }
    void setValue(uint8_t value)
    {
        if(value>=0x80)
        {
            if(value >= 0xC0)
                setQuadrantAngle(value, LedG, LedB, LedR, LedO);
            else
                setQuadrantAngle(value, LedB, LedR, LedO, LedG);
        }
        else
        {
            if(value >= 0x40)
                setQuadrantAngle(value, LedR, LedO, LedG, LedB);
            else
                setQuadrantAngle(value, LedO, LedG, LedB, LedR);
        }
    }
private:
    //On-board leds
/*
 *      O
 *  G        R
 *      B
 */
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedG;
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedO;
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedR;
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedB;
    void setQuadrantAngle(uint8_t value,
                          Pwm< GeneralPurposeTimer< uint16_t, int(4) > >& A,
                          Pwm< GeneralPurposeTimer< uint16_t, int(4) > >& B,
                          Pwm< GeneralPurposeTimer< uint16_t, int(4) > >& C,
                          Pwm< GeneralPurposeTimer< uint16_t, int(4) > >& D
                         )
    {
        uint8_t v = (value & 0x3F)<<2;
        A.setDutyCycle((float)((float)v/256.0));
        B.setDutyCycle((float)(1.0-((float)v/256.0)));
        C.setDutyCycle(0);
        D.setDutyCycle(0);
    }

};

int main()
{
    DiscoveryBoard board;
    RTOS::Time t(board.Tim7);
    LedAngleDisplay display (board);
    Platform::Gpio scl(board.GpioB[6]);
    Platform::Gpio sda(board.GpioB[7]);
    Platform::I2C bus(scl, sda, Platform::I2C::I2C_1);
    uint8_t target_reg = 0x01;
    uint8_t compass_address = 0b01100000;
    while(1)
    {
        bus.write(compass_address, &target_reg, 1);
        uint8_t data;
        t.msleep(10);
        bus.read(compass_address, &data, 1);
        display.setValue(data);
        t.msleep(100);
    }
}