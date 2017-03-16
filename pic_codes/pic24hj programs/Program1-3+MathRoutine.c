/*===============================================================================
 Program1-3+MathRoutine.c 

 This program reads a binary input (0 to 7 in decimal) on PORTA (RA2,RA3,RA4).
 The square of this value is calculated using a multiplication with the result 
 sent as a binary value to the 8 output pins which have been configured on PORTB 
 and connected to LEDs. This program loops indefinitely (read input, find result 
 and display on LEDs)
===============================================================================*/

#include "p24HJ128GP502.h"      //Includes device library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC   (POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT   (FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR   (FPWRT_PWR1 & ALTI2C_ON);
_FICD   (ICS_PGD1 & JTAGEN_OFF);

int main(void)
{
    unsigned int Input = 0;     //Initialize the variable Input to 0;
        
    TRISA = 0xFFFF;             //Set all PORTA pins as inputs
    TRISB = 0;                  //Set all PORTB pins as outputs
                
    while(1)
    {
        Input = PORTA >> 2;     //Set Input equal to the state of PORTA pins 
                                //and shift it by 2

/* NOTE: In this program PORTA is a register that holds the information describing the 
state of each pin.  If it's reading a HIGH input then its value is 1, and if it's 
reading a LOW input then the value is 0. If pins RA2, RA3 and RA4 are ON, or 
equal to 1, then the binary value of PORTA register would be 00000000 00011100 
which is equal to 28 (but we want it to equal 7). Therefore by shifting it by 2 
now we have 00000000 00000111 which is equal to 7.*/
                
        Input = Input * Input;  //Calculate the square value of the variable Input 

        LATB = Input << 4;      /*Display the binary value of the variable Input
                                  to PORTB register which is connected to LEDs.
                                  Like before we are shifting by four to skip
                                  pins RB0, RB1, RB2, and RB3.*/
    }
}

