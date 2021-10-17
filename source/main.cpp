#include "MicroBit.h"

// Global microbit structure for interfacing with the microbit board
MicroBit uBit;

/*
 * FEATURE FLAGS
 */

// If set will alternate directions at intersections, otherwise will go straight through
#define INTERSECTION_ALTERNATE 1

// If set will enable the sonar in the robot
#define USE_SONAR 0

/*
 * HARDWARE BASED FUNCTIONS
 */

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
        uBit.io.P8.setDigitalValue(on ? 1 : 0);
    else if (led == LED::LED_RIGHT || led == LED::LED_ALL)
        uBit.io.P12.setDigitalValue(on ? 1 : 0);
}

// enum for addressing the two linesensors
enum LineSensor
{
    LS_LEFT = 1,
    LS_RIGHT = 2,
};

// readLine reads the value at the given line sensor
int readLine(LineSensor line)
{
    if (line == LineSensor::LS_LEFT)
        return uBit.io.P13.getDigitalValue();
    else if (line == LineSensor::LS_RIGHT)
        return uBit.io.P14.getDigitalValue();
    return -1;
}

// Enum for addressing each of the two wheels.
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

#if USE_SONAR
// does a cycle of the ultrasonic reader, returning the distance in cm
int readUlt()
{
    // send a pulse out. Timings found on a microbit/maqueen reference
    uBit.io.P1.setDigitalValue(0);
    uBit.sleep(2);
    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(10);
    uBit.io.P1.setDigitalValue(0);

    // listen for a high pulse on the second pin
    uBit.io.P2.setPolarity(1);
    int d = uBit.io.P2.getPulseUs(250);
    return d > 0 ? d / 58 : d;
}
#endif

/*
 *
 * GLOBAL STATE
 *
 */

// different states the robot will be in
enum State
{
    FOLLOW_LINE = 0,
    STOPPED = 1,
    COMPUTING = 2,
};

// current state of the robot
State state = FOLLOW_LINE;
bool recompute = false;

// current values of the line sensors. These are updated by the
// line sensor events
int8_t leftLS = 0;
int8_t rightLS = 0;

// level of abstraction on what the robot thinks the directions are.
// allows us to modify these values to make the robot go backwards instead
// of forward for example
WheelDirection forward = WHEEL_FORWARD;
WheelDirection reverse = WHEEL_BACKWARD;
Wheel left = WHEEL_LEFT;
Wheel right = WHEEL_RIGHT;

// if true, goes right at an intersection. Otherwise it goes left.
// only need this value if doing the intersection logic
#if INTERSECTION_ALTERNATE
bool goRightAtIntersection = false;
#endif

/*
 *
 * DOMAIN LOGIC
 *
 */

// This function is responsible for all of the driving logic. All of the events call this function
// to create a response to the state change the robot may have given
void drive()
{
    // currently stopped - ignore event
    if (state == State::STOPPED)
        return;

    // another routine is running - set recompute flag and return
    if (state == State::COMPUTING)
    {
        recompute = true;
        return;
    }
    state = State::COMPUTING;

    if (!leftLS && !rightLS)
    {
        moveWheel(left, 28, forward);
        moveWheel(right, 28, forward);
    }
#define ADJUST_SPEED 20 // put an inline define to make tweaking values quicker
    else if (leftLS && !rightLS)
    {
        moveWheel(left, ADJUST_SPEED, forward);
        moveWheel(right, ADJUST_SPEED, reverse);
    }
    else if (!leftLS && rightLS)
    {
        moveWheel(right, ADJUST_SPEED, forward);
        moveWheel(left, ADJUST_SPEED, reverse);
    }
    else
    {

        // go forward to test if its an intersection
        moveWheel(left, 24, forward);
        moveWheel(right, 24, forward);
        uBit.sleep(350);

        if (!readLine(LS_LEFT) || !readLine(LS_RIGHT))
        {
            // We think this is an intersection, perform that logic
            moveWheel(left, 0, forward);
            moveWheel(right, 0, forward);
            uBit.sleep(2000);

#if INTERSECTION_ALTERNATE
            if (goRightAtIntersection)
            {
                moveWheel(left, 30, forward);
                moveWheel(right, 25, reverse);
            }
            else
            {
                moveWheel(right, 30, forward);
                moveWheel(left, 25, reverse);
            }
            uBit.sleep(800);

            // toggle the direction we choose
            goRightAtIntersection = !goRightAtIntersection;
#else
            moveWheel(left, speed, forward);
            moveWheel(right, speed, forward);
#endif
        }
        else
        {
            // This isn't an intersection, try treat it as a sharp corner

            // Move back to the line, we moved forward just before to test if this was an
            // intersection. I found it works best to not move all the way back
            moveWheel(left, 24, reverse);
            moveWheel(right, 24, reverse);
            uBit.sleep(200);

            // move the robot to the left a bit. If we find a line then assume the corner goes that
            // way and follow it around
            moveWheel(right, 30, forward);
            moveWheel(left, 30, reverse);
            uBit.sleep(700);

            // maybe sample a few times over the time period so we can check if we passed over the
            // line?

            // if both line sensors are true then we aren't on a line, move back
            // this will leave the robot in the turning state so worst case, it
            // will turn all the way back to the line it was on before. This accounts for the end of
            // line states
            if (readLine(LS_LEFT) && readLine(LS_RIGHT))
            {
                moveWheel(left, 30, forward);
                moveWheel(right, 30, reverse);
            }
        }
    }
    state = State::FOLLOW_LINE;
    if (recompute)
    {
        recompute = false;
        drive();
    }
}

/*
 *
 * EVENT HANDLERS
 *
 */

void onLeftLineOn(MicroBitEvent evt)
{
    setLED(LED::LED_LEFT, true);
    leftLS = 1;
    drive();
}

void onLeftLineOff(MicroBitEvent evt)
{
    setLED(LED::LED_LEFT, false);
    leftLS = 0;
    drive();
}

void onRightLineOn(MicroBitEvent evt)
{
    setLED(LED::LED_RIGHT, true);
    rightLS = 1;
    drive();
}

void onRightLineOff(MicroBitEvent evt)
{
    setLED(LED::LED_RIGHT, false);
    rightLS = 0;
    drive();
}

#if USE_SONAR
// gets called every 250ms
void timerTick(MicroBitEvent)
{
    if (state != State::FOLLOW_LINE && state != State::STOPPED)
        return;

    // read the ultrasonic sensor
    int d = readUlt();

    // only do something if a valid value was read, is negative if not able to determine
    if (d > 0)
    {
        if (state == State::STOPPED && d >= 10)
            state = State::FOLLOW_LINE;

        if (state != State::STOPPED && (5 <= d && d < 10))
        {
            moveWheel(WHEEL_LEFT, 0, WHEEL_FORWARD);
            moveWheel(WHEEL_RIGHT, 0, WHEEL_FORWARD);
            state = State::STOPPED;
        }

        if (forward != (d < 5 ? WHEEL_BACKWARD : WHEEL_FORWARD))
        {
            if (state == State::STOPPED)
                state = State::FOLLOW_LINE;

            if (d < 5)
            {
                forward = WHEEL_BACKWARD;
                reverse = WHEEL_FORWARD;
                left = WHEEL_RIGHT;
                right = WHEEL_LEFT;
            }
            else
            {
                forward = WHEEL_FORWARD;
                reverse = WHEEL_BACKWARD;
                left = WHEEL_LEFT;
                right = WHEEL_RIGHT;
            }
        }
    }
}
#endif

// The main function registers the event handlers and then lets microbit handle things from there
int main()
{
    uBit.init();

    // register handlers for the line sensor events
    uBit.io.P13.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.io.P14.eventOn(DEVICE_PIN_EVENT_ON_EDGE);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_RISE, onLeftLineOn);
    uBit.messageBus.listen(uBit.io.P13.id, DEVICE_PIN_EVT_FALL, onLeftLineOff);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_RISE, onRightLineOn);
    uBit.messageBus.listen(uBit.io.P14.id, DEVICE_PIN_EVT_FALL, onRightLineOff);

#if USE_SONAR
    // register an event to occur every 250 ms. We use this for the ultrasonic sensor.
    uBit.timer.eventEvery(250, 5000, DEVICE_EVT_ANY, CODAL_TIMER_EVENT_FLAGS_NONE);
    uBit.messageBus.listen(5000, DEVICE_EVT_ANY, timerTick);
#endif

    // bootstrap the movements by calling drive()
    // this should just make the robot go straight forward
    drive();

    release_fiber();
}
