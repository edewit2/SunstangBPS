/*===============================================================================
 Program1-2.c

 This program loops a specified number of times by using a count-down scheme.
 The loop is controlled by the value stored in the user variable "Count", which
 is decremented by 1 during each iteration.  The binary value of "Count" is then
 displayed on PORTB.  When "Count" = 0, the loop is finished and the program will
 reset "Count" = 7 and run again (in an infinite loop).
 
===============================================================================*/

#define FCY     3685000     //Defines device instruction frequency which is needed
                            //for Software Delay Routine (i.e. __delay_ms(250)
#include "p24HJ128GP502.h"  //Includes device library file
#include "libpic30.h"       //Includes delay library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF)
_FOSC   (POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT   (FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR   (FPWRT_PWR1 & ALTI2C_ON);
_FICD   (ICS_PGD1 & JTAGEN_OFF);

int main(void)
{
    unsigned int Count = 7; //Initialize "Count" to 7
                            //(Count is used to decrement LEDs)
    TRISB = 0;              //Set all PORTB pins as outputs

    while (1)               //Program will run forever in an infinite loop
    {       
        LATB = Count << 4;  //Output Count to PORTB pins (RB4, RB5, RB6) by shifting
                            //4 bits which will skip the first 4 pins in order to
                            //display the binary output properly on LEDs
        
        __delay_ms(250);    //Software delay is used to keep LEDs on for 250ms
                            //Note-software delay may not be the best method of 
                            //timing, it's often better to use a Timer interrupt
                            //(refer to Program1-1+Interrupts.c )

        while (Count > 0)   //Checks if Count is greater than 0; if it is then continue
        {
            Count--;            //Decrements Count by 1
            LATB = Count << 4;  //Shift Count by 4 bits and display on LEDs in binary 
            __delay_ms(250);    //Software delay of 250ms
        }
        Count = 7;              //Count is reset to 7
    }
}

