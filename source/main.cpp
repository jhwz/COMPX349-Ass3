#include "MicroBit.h"

MicroBit uBit;



int  ret;  

void displayStatus(char i){
    uBit.display.image.clear();
    if (i == "f"){
        uBit.display.image.setPixelValue(0,2,255);
        uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(2,0,255);
        uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);
    }
    else if (i == "b"){
        uBit.display.image.setPixelValue(0,2,255);
        uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(2,0,255);
        uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);
    }
    else if (i == "l"){
        uBit.display.image.setPixelValue(0,2,255);
        uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(2,0,255);
        uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);
    }
    else if (i == "r"){
        uBit.display.image.setPixelValue(0,2,255);
        uBit.display.image.setPixelValue(1,1,255);
        uBit.display.image.setPixelValue(2,0,255);
        uBit.display.image.setPixelValue(3,1,255);
        uBit.display.image.setPixelValue(4,2,255);
    }
}

void forward(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    uBit.display.print("F");
}
void reverse(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x01;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x01;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    uBit.display.print("R");
}
void right(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x00; 
    uBit.i2c.write( 0x20, buf, 3); 
    uBit.display.print("<");
}
void left(){
    uint8_t buf[3]; 
    buf[0] = 0x00;buf[1] = 0x00;buf[2] = 0x00; 
    uBit.i2c.write( 0x20, buf, 3); 
    buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
    uBit.display.print(">");
}



int main()
{
    uBit.init();

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
}
