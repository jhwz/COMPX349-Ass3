#include "MicroBit.h"
// #include "robot.cpp"

#define INTERSECTION_ALTERNATE 1

MicroBit uBit;

// Enum for all of the LED types. There are two LEDs on the maqueen.
enum LED
{
    LED_ALL = 0,
    LED_LEFT = 1,
    LED_RIGHT = 2,
};

// Allows you to set a LED either on or off
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

// enum for addressing the two linesensors
enum LineSensor
{
    LS_LEFT = 1,
    LS_RIGHT = 2,
};

int readLine(LineSensor line)
{
    if (line == LineSensor::LS_LEFT)
        return uBit.io.P13.getDigitalValue();
    else if (line == LineSensor::LS_RIGHT)
        return uBit.io.P14.getDigitalValue();
    return -1;
}

// Enum for addressing the wheels.
enum Wheel
{
    WHEEL_LEFT = 0x00,
    WHEEL_RIGHT = 0x02,
};

// Enum for saying which direction the wheels should go in
enum WheelDirection
{
    WHEEL_FORWARD = 0x0,
    WHEEL_BACKWARD = 0x1,
};

// allows you to move the specified wheel in the given direction for some speed value between 0 and
// 255
void moveWheel(Wheel wheel, uint8_t speed, WheelDirection dir)
{
    uint8_t buf[3];
    buf[0] = (uint8_t)wheel;
    buf[1] = dir;
    buf[2] = speed;
    uBit.i2c.write(0x20, buf, 3);
}

// does a cycle of the ultrasonic reader, returning the distance in cm
int readUlt()
{
    int d;
    uBit.io.P1.setDigitalValue(0);
    uBit.sleep(2);
    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(10);
    uBit.io.P1.setDigitalValue(0);

    uBit.io.P2.setPolarity(1);
    d = uBit.io.P2.getPulseUs(250);
    return d > 0 ? d / 58 : d;
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

    // intersection detection
    bool searching;
    unsigned long enableIntersectionsAfter;

#if INTERSECTION_ALTERNATE
    // if true, goes right at an intersection. Otherwise it goes left
    bool goRightAtIntersection;
#endif
};

State state = {};

void drive()
{
    // no driving logic - currently doing intersection detection
    if (state.searching)
        return;

    if (state.stopped)
    {
        moveWheel(WHEEL_LEFT, 0, WHEEL_FORWARD);
        moveWheel(WHEEL_RIGHT, 0, WHEEL_FORWARD);
        state.prevLeftLS = -1;
        state.prevRightLS = -1;
        return;
    }
    else if (state.prevLeftLS == state.leftLS && state.prevRightLS == state.rightLS)
        return;

    if (!state.leftLS && !state.rightLS)
    {
        moveWheel(state.left, 32, state.forward);
        moveWheel(state.right, 32, state.forward);
    }
    else if (state.leftLS && !state.rightLS)
    {
        moveWheel(state.left, 24, state.forward);
        moveWheel(state.right, 0, state.reverse);
    }
    else if (!state.leftLS && state.rightLS)
    {
        moveWheel(state.right, 24, state.forward);
        moveWheel(state.left, 0, state.reverse);
    }
    else
    {
        // Disable intersection detection for a bit
        if (uBit.systemTime() < state.enableIntersectionsAfter)
            return;

        uBit.serial.printf("intersection detection\n\r");

        // try detect an intersection
        bool intersection = false;
        state.searching = true;
        const int speed = 28;
        const int sleep = 300;

        // go forward to test if its an intersection
        moveWheel(state.left, speed, state.forward);
        moveWheel(state.right, speed, state.forward);
        uBit.sleep(sleep);
        intersection = !readLine(LS_LEFT) || !readLine(LS_RIGHT);
        if (intersection)
        {
#if INTERSECTION_ALTERNATE
            moveWheel(state.left, speed, state.reverse);
            moveWheel(state.right, speed, state.reverse);
            uBit.sleep(sleep);

            if (state.goRightAtIntersection)
            {
                moveWheel(state.left, speed, state.forward);
                moveWheel(state.right, speed, state.reverse);
            }
            else
            {
                moveWheel(state.right, speed, state.forward);
                moveWheel(state.left, speed, state.reverse);
            }
            uBit.sleep(500);
            moveWheel(state.right, speed, state.forward);
            moveWheel(state.left, speed, state.forward);
            uBit.sleep(300);
            state.goRightAtIntersection = !state.goRightAtIntersection;
#else
            moveWheel(state.left, speed, state.forward);
            moveWheel(state.right, speed, state.forward);
#endif
        }
        else
        {
            moveWheel(state.left, speed, state.reverse);
            moveWheel(state.right, speed, state.reverse);
            uBit.sleep(sleep);

            // try go right
            moveWheel(state.right, speed, state.forward);
            moveWheel(state.left, speed, state.reverse);
            uBit.sleep(sleep * 2);
            moveWheel(state.right, 0, state.forward);
            moveWheel(state.left, 0, state.reverse);
            if (readLine(LS_LEFT) && readLine(LS_RIGHT))
            {
                moveWheel(state.left, speed, state.forward);
                moveWheel(state.right, speed, state.reverse);
                uBit.sleep(sleep * 2);
            }
            else
            {
                moveWheel(state.right, speed, state.forward);
                moveWheel(state.left, speed, state.reverse);
            }
        }

        // only bother doing this again after a second
        state.enableIntersectionsAfter = uBit.systemTime() + 1000;
        state.searching = false;
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
    // uBit.serial.printf("ultrasonic: %d\n\r", d);
    if (d > 0)
    {
        bool callDrive = false;
        if (state.stopped != (5 <= d && d < 10))
        {
            state.stopped = !state.stopped;
            callDrive = true;
        }
        if (state.forward != (d < 5 ? WHEEL_BACKWARD : WHEEL_FORWARD))
        {
            if (d < 5)
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

    // explicitly initilise state
    state.prevLeftLS = -1;
    state.prevRightLS = -1;
    state.forward = WHEEL_FORWARD;
    state.reverse = WHEEL_BACKWARD;
    state.left = WHEEL_LEFT;
    state.right = WHEEL_RIGHT;

    // register handlers for our events
    uBit.io.P13.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.io.P14.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_RISE, onLeftLineOn);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_FALL, onLeftLineOff);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_RISE, onRightLineOn);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_FALL, onRightLineOff);

    uBit.timer.eventEvery(250, 5000, DEVICE_EVT_ANY, CODAL_TIMER_EVENT_FLAGS_NONE);
    uBit.messageBus.listen(5000, DEVICE_EVT_ANY, timerTick);

    release_fiber();
}
