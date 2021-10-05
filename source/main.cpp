#include "MicroBit.h"

MicroBit uBit;

#define INTERSECTION_ALTERNATE 1

enum LED
{
    LED_ALL = 0,
    LED_LEFT = 1,
    LED_RIGHT = 2,
};

void setLED(LED led, bool on)
{
    if (led == LED::LED_LEFT || led == LED::LED_ALL)
    {
        uBit.io.P8.setDigitalValue(on ? 1 : 0);
    }
    else if (led == LED::LED_RIGHT || led == LED::LED_ALL)
    {
        uBit.io.P12.setDigitalValue(on ? 1 : 0);
    }
}

enum Line
{
    LINE_LEFT = 1,
    LINE_RIGHT = 2,
};

int readLine(Line line)
{
    if (line == Line::LINE_LEFT)
    {
        return uBit.io.P13.getDigitalValue();
    }
    else if (line == Line::LINE_RIGHT)
    {
        return uBit.io.P14.getDigitalValue();
    }
    return -1;
}

enum Wheel
{
    WHEEL_LEFT = 0x00,
    WHEEL_RIGHT = 0x02,
};

enum WheelDirection
{
    WHEEL_FORWARD = 0x0,
    WHEEL_BACKWARD = 0x1,
};

void moveWheel(Wheel wheel, uint8_t speed, WheelDirection dir)
{
    uint8_t buf[3];
    buf[0] = (uint8_t)wheel;
    buf[1] = dir;
    buf[2] = speed;
    uBit.i2c.write(0x20, buf, 3);
}

int readUlt()
{
    int d;
    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(1);
    uBit.io.P1.setDigitalValue(0);
    if (uBit.io.P2.getDigitalValue() == 0)
    {
        uBit.io.P1.setDigitalValue(0);
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(0);
        uBit.io.P2.setPolarity(1);
        d = uBit.io.P2.getPulseUs(50);
    }
    else
    {
        uBit.io.P1.setDigitalValue(1);
        uBit.io.P1.setDigitalValue(0);
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(0);
        uBit.io.P2.setPolarity(0);
        d = uBit.io.P2.getPulseUs(50);
    }
    return d;
}

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

struct State
{
    int8_t prevLeftLS;
    int8_t prevRightLS;
    int8_t leftLS;
    int8_t rightLS;

    bool stopped;

    WheelDirection forward;
    WheelDirection reverse;
    Wheel left;
    Wheel right;

#if INTERSECTION_ALTERNATE
    // if true, goes right at an intersection. Otherwise it goes left
    bool goRightAtIntersection;
#endif
};

State state = {};

void drive()
{
    if (state.stopped)
    {
        moveWheel(WHEEL_LEFT, 0, WHEEL_FORWARD);
        moveWheel(WHEEL_RIGHT, 0, WHEEL_FORWARD);
        state.prevLeftLS = -1;
        state.prevRightLS = -1;
        return;
    }

    if (state.prevLeftLS == state.leftLS && state.prevRightLS == state.rightLS)
        return;

    uBit.serial.printf("updating trajectory\n\r");
    if (!state.leftLS && !state.rightLS)
    {
        moveWheel(state.left, 32, state.forward);
        moveWheel(state.right, 32, state.forward);
    }
    else if (state.leftLS && !state.rightLS)
    {
        moveWheel(state.left, 24, state.forward);
        moveWheel(state.right, 0, state.forward);
    }
    else if (!state.leftLS && state.rightLS)
    {
        moveWheel(state.right, 24, state.forward);
        moveWheel(state.left, 0, state.forward);
    }
    else
    {
#if INTERSECTION_ALTERNATE
        if (state.goRightAtIntersection)
        {
            moveWheel(state.right, 24, state.forward);
            moveWheel(state.left, 24, state.reverse);
        }
        else
        {
            moveWheel(state.left, 24, state.forward);
            moveWheel(state.right, 24, state.reverse);
        }
        state.goRightAtIntersection = !state.goRightAtIntersection;
#else
        moveWheel(state.left, 24, state.forward);
        moveWheel(state.right, 24, state.forward);
#endif
    }
    state.prevLeftLS = state.leftLS;
    state.prevRightLS = state.rightLS;
}

//
// Events
//

void onLeftLineOn(MicroBitEvent evt)
{
    setLED(LED::LED_LEFT, true);
    state.leftLS = 1;
    drive();
}

void onLeftLineOff(MicroBitEvent evt)
{
    setLED(LED::LED_LEFT, false);
    state.leftLS = 0;
    drive();
}

void onRightLineOn(MicroBitEvent evt)
{
    setLED(LED::LED_RIGHT, true);
    state.rightLS = 1;
    drive();
}

void onRightLineOff(MicroBitEvent evt)
{
    setLED(LED::LED_RIGHT, false);
    state.rightLS = 0;
    drive();
}

void timerTick(MicroBitEvent)
{
    int d = readUlt();
    if (d > 0)
    {
        bool callDrive = false;
        if (state.stopped != (300 <= d && d < 400))
        {
            state.stopped = !state.stopped;
            callDrive = true;
        }
        if (state.forward != (d < 300 ? WHEEL_BACKWARD : WHEEL_FORWARD))
        {
            if (d < 300)
            {
                state.forward = WHEEL_BACKWARD;
                state.reverse = WHEEL_FORWARD;
                state.left = WHEEL_RIGHT;
                state.right = WHEEL_LEFT;
            }
            else
            {
                state.forward = WHEEL_FORWARD;
                state.reverse = WHEEL_BACKWARD;
                state.left = WHEEL_LEFT;
                state.right = WHEEL_RIGHT;
            }
            callDrive = true;
        }
        if (callDrive)
            drive();
    }
}

int main()
{
    uBit.init();
    uBit.serial.printf("starting\n\r");

    state.prevLeftLS = -1;
    state.prevRightLS = -1;
    state.forward = WHEEL_FORWARD;
    state.reverse = WHEEL_BACKWARD;
    state.left = WHEEL_LEFT;
    state.right = WHEEL_RIGHT;

    uBit.io.P13.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.io.P14.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_RISE, onLeftLineOn);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_FALL, onLeftLineOff);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_RISE, onRightLineOn);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_FALL, onRightLineOff);

    uBit.timer.eventEvery(10, 5000, DEVICE_EVT_ANY, CODAL_TIMER_EVENT_FLAGS_NONE);
    uBit.messageBus.listen(5000, DEVICE_EVT_ANY, timerTick);

    release_fiber();
}
