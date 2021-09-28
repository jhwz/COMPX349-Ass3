#include "MicroBit.h"

MicroBit uBit;



int main()
{
    uBit.init();
    uint8_t buf[3]; 
    int  ret;  

    buf[0] = 0x00;
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x80;  
    
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;
    ret = uBit.i2c.write( 0x20, buf, 3); 
        
    uBit.display.scroll(ret);
}
