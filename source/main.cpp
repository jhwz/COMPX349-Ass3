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

enum Line
{
    LEFTSensor = 1,
    RIGHTSensor = 2,
};

int readLine(Line line)
{
    if (line == Line::LEFTSensor)
    {
        return uBit.io.P13.getDigitalValue();
    }
    else if (line == Line::RIGHTSensor)
    {
        return uBit.io.P14.getDigitalValue();
    }
    return -1;
}

int main()
{
    uBit.init();

    for (;;)
    {
        if (readLine(Line::LEFTSensor) == 1)
        {
            setLED(LED::LEFT, true);
        }
        else
        {
            setLED(LED::LEFT, false);
        }
        if (readLine(Line::RIGHTSensor) == 1)
        {
            setLED(LED::RIGHT, true);
        }
        else
        {
            setLED(LED::RIGHT, false);
        }
    }
}
