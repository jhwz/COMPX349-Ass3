#include "MicroBit.h"

MicroBit uBit;

void drive();

// event handelers
#define CHANGE_PATH MICROBIT_ID_NOTIFY + 1
#define CHANGE_PATH_EVT 0

int leftLT = 0;
int rightLT = 0;

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

int ret;

void displayStatus(char i)
{
    uBit.display.image.clear();
    if (i == 'f')
    {
        uBit.display.image.setPixelValue(0, 2, 255);
        uBit.display.image.setPixelValue(1, 1, 255);
        uBit.display.image.setPixelValue(2, 0, 255);
        uBit.display.image.setPixelValue(3, 1, 255);
        uBit.display.image.setPixelValue(4, 2, 255);
        uBit.display.image.setPixelValue(2, 1, 255);
        uBit.display.image.setPixelValue(2, 2, 255);
        uBit.display.image.setPixelValue(2, 3, 255);
        uBit.display.image.setPixelValue(2, 4, 255);
    }
    else if (i == 'b')
    {
        uBit.display.image.setPixelValue(0, 2, 255);
        uBit.display.image.setPixelValue(1, 3, 255);
        uBit.display.image.setPixelValue(2, 4, 255);
        uBit.display.image.setPixelValue(3, 3, 255);
        uBit.display.image.setPixelValue(4, 2, 255);
        uBit.display.image.setPixelValue(2, 1, 255);
        uBit.display.image.setPixelValue(2, 2, 255);
        uBit.display.image.setPixelValue(2, 3, 255);
    }
    else if (i == 'l')
    {
        uBit.display.image.setPixelValue(2, 0, 255);
        uBit.display.image.setPixelValue(3, 1, 255);
        uBit.display.image.setPixelValue(4, 2, 255);
        uBit.display.image.setPixelValue(3, 3, 255);
        uBit.display.image.setPixelValue(2, 4, 255);
        uBit.display.image.setPixelValue(1, 2, 255);
        uBit.display.image.setPixelValue(2, 2, 255);
        uBit.display.image.setPixelValue(3, 2, 255);
        uBit.display.image.setPixelValue(0, 2, 255);
    }
    else if (i == 'r')
    {
        uBit.display.image.setPixelValue(2, 0, 255);
        uBit.display.image.setPixelValue(1, 1, 255);
        uBit.display.image.setPixelValue(0, 2, 255);
        uBit.display.image.setPixelValue(1, 3, 255);
        uBit.display.image.setPixelValue(2, 4, 255);
        uBit.display.image.setPixelValue(1, 2, 255);
        uBit.display.image.setPixelValue(2, 2, 255);
        uBit.display.image.setPixelValue(3, 2, 255);
        uBit.display.image.setPixelValue(4, 2, 255);
    }
}

void forward()
{
    uint8_t buf[3];
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x30;
    uBit.i2c.write(0x20, buf, 3);
    buf[0] = 0x02;
    buf[1] = 0x00;
    buf[2] = 0x30;
    uBit.i2c.write(0x20, buf, 3);
    displayStatus('f');
}
void reverse()
{
    uint8_t buf[3];
    buf[0] = 0x00;
    buf[1] = 0x01;
    buf[2] = 0x40;
    uBit.i2c.write(0x20, buf, 3);
    buf[0] = 0x02;
    buf[1] = 0x01;
    buf[2] = 0x40;
    uBit.i2c.write(0x20, buf, 3);
    displayStatus('b');
}
void right()
{
    uint8_t buf[3];
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x20;
    uBit.i2c.write(0x20, buf, 3);
    buf[0] = 0x02;
    buf[1] = 0x01;
    buf[2] = 0x20;
    uBit.i2c.write(0x20, buf, 3);
    displayStatus('r');
}
void left()
{
    uint8_t buf[3];
    buf[0] = 0x00;
    buf[1] = 0x10;
    buf[2] = 0x20;
    uBit.i2c.write(0x20, buf, 3);
    buf[0] = 0x02;
    buf[1] = 0x00;
    buf[2] = 0x20;
    uBit.i2c.write(0x20, buf, 3);
    displayStatus('l');
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

void onLeftLineOn(MicroBitEvent evt)
{
    setLED(LED::LEFT, true);
    leftLT = 1;
    drive();
}

void onLeftLineOff(MicroBitEvent evt)
{
    setLED(LED::LEFT, false);
    leftLT = 0;
    drive();
}

void onRightLineOn(MicroBitEvent evt)
{
    setLED(LED::RIGHT, true);
    rightLT = 1;
    drive();
}

void onRightLineOff(MicroBitEvent evt)
{
    setLED(LED::RIGHT, false);
    rightLT = 0;
    drive();
}

void drive(){
    uBit.serial.printf("updateing trajectory\n\r");
    if (!leftLT && rightLT){forward();}
    if (!leftLT && !rightLT){right();}
    if (leftLT && rightLT){left();}
}

int main()
{
    uBit.init();
    uBit.serial.printf("starting\n\r");
    uBit.io.P13.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.io.P14.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_RISE, onLeftLineOn);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_FALL, onLeftLineOff);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_RISE, onRightLineOn);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_FALL, onRightLineOff);
    //uBit.messageBus.listen(CHANGE_PATH MICROBIT, CHANGE_PATH_EVT, drive);
    release_fiber();
}
