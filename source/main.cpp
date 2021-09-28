#include "MicroBit.h"

MicroBit uBit;

enum LED
{
    ALL = 0,
    LEFT = 1,
    RIGHT = 2,
};

void setLED(LED led, bool on)
{
    if (led == LED::LEFT || led == LED::ALL)
    {
        uBit.io.P8.setDigitalValue(on ? 1 : 0);
    }
    else if (led == LED::RIGHT || led == LED::ALL)
    {
        uBit.io.P12.setDigitalValue(on ? 1 : 0);
    }
}

int main()
{
    uBit.init();

    setLED(LED::ALL, true);
}
