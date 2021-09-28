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

<<<<<<< HEAD
int  ret;  

void displayStatus(char i){
    uBit.display.image.clear();
    if (i == 'f'){
        uBit.display.image.setPixelValue(0,2,255);uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(2,0,255);uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);uBit.display.image.setPixelValue(2,1,255);
        uBit.display.image.setPixelValue(2,2,255);uBit.display.image.setPixelValue(2,3,255);
    }
    else if (i == 'b'){
        uBit.display.image.setPixelValue(0,2,255);uBit.display.image.setPixelValue(1,3,255);     
        uBit.display.image.setPixelValue(2,4,255);uBit.display.image.setPixelValue(3,3,255);        
        uBit.display.image.setPixelValue(4,2,255);uBit.display.image.setPixelValue(2,1,255);
        uBit.display.image.setPixelValue(2,2,255);uBit.display.image.setPixelValue(2,3,255);
    }
    else if (i == 'l'){
        uBit.display.image.setPixelValue(2,0,255);uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);uBit.display.image.setPixelValue(3,3,255);
        uBit.display.image.setPixelValue(2,4,255);uBit.display.image.setPixelValue(1,2,255);
        uBit.display.image.setPixelValue(2,2,255);uBit.display.image.setPixelValue(3,2,255);
    }
    else if (i == 'r'){
        uBit.display.image.setPixelValue(2,0,255);uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(0,2,255);uBit.display.image.setPixelValue(1,3,255);
        uBit.display.image.setPixelValue(2,4,255);uBit.display.image.setPixelValue(1,2,255);
        uBit.display.image.setPixelValue(2,2,255);uBit.display.image.setPixelValue(3,2,255);
    }
}

void forward(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x60; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x60; 
    uBit.i2c.write( 0x20, buf, 3); 
    displayStatus('f');
}
void reverse(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x01;buf[2] = 0x60; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x01;buf[2] = 0x60; 
    uBit.i2c.write( 0x20, buf, 3); 
    displayStatus('b');
}
void right(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x00; 
    uBit.i2c.write( 0x20, buf, 3); 
    displayStatus('r');
}
void left(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x00; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    displayStatus('l');
}


=======
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
>>>>>>> c7b3bd0efd7079ed18a606919c54359f66842f9b

int main()
{
    uBit.init();

<<<<<<< HEAD
    uint8_t buf[3]; 
    buf[0] = 0x00;//motor selection x02 right x00 left
    buf[1] = 0x00;//motor direction 0 forward,  1 backward
    buf[2] = 0x00;//motor speed
    //uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    //buf[0] = 0x02;
    //ret = uBit.i2c.write( 0x20, buf, 3); 
    while(1){
    	forward();
    	uBit.sleep(2000);
    	reverse();
    	uBit.sleep(2000);
    	right();
    	uBit.sleep(2000);
    	left();
    	uBit.sleep(2000);
    }
    uBit.display.scroll(ret);
=======
    setLED(LED::ALL, true);
>>>>>>> 8d4910d7183c8d9207eacf91e403f95b71f92517
=======
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
>>>>>>> c7b3bd0efd7079ed18a606919c54359f66842f9b
}
