
#include <stdio.h>
#include <stdlib.h>
#include <delays.h>
#include "SunstangBPS_temp.h"

/* 
This program counts up to 15 in binary and resets. Data is displayed on PORTB.
 */
void main(void) {

    TRISB = 0;                    // Set PORTB as output
    
    unsigned int muxcount = 0;
    
    while(1)
    {
        LATB = muxcount;    //Output Count to PORTB pins (RB0, RB1, RB2, RB3)

        __delay_ms(500);    //Software delay is used to keep LEDs on for 150ms
                            //Note-software delay may not be the best method of 
                            //timing, it's often better to use a Timer interrupt
                            //(refer to Program1-1+Interrupts.c )

        while (muxcount < 15)   //Routine checks if Count is less than 11; if it is continue
        {
            muxcount++;            //Increments Count by 1
            LATB = muxcount;  // Display on LEDs in binary 
            __delay_ms(500);       // Software delay of 150ms
        }       
        muxcount = 0;              //Count is reset to 0
        
    }
    
}