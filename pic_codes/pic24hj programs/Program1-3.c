/*===============================================================================
 Program1-3.c

 This program reads a binary input (0 to 7 in decimal) on PORTA (RA2,RA3,RA4) and
 uses conditional statements to find the square of this value. The result is
 then sent as a binary value to the 8 output pins which have been configured on
 PORTB (RB4, RB5, RB6, RB7, RB8, RB9, RB10, RB11) and connected to LEDs.
 This program loops indefinitely (read input, find result in table, and display)
 
===============================================================================*/

#include "p24HJ128GP502.h"      //Includes device library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF)
_FOSC   (POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT   (FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR   (FPWRT_PWR1 & ALTI2C_ON);
_FICD   (ICS_PGD1 & JTAGEN_OFF);

int main(void)
{
    unsigned int Input = 0;     //Initialize the variable "Input" to 0;
        
    TRISA = 0xFFFF;             //Set all PORTA pins as inputs (hex notation)
    TRISB = 0;                  //Set all PORTB pins as outputs
                
    while(1)
    {
        Input = PORTA >> 2;     //Set Input equal to the state of PORTA pins and shift 
                                //it right by 2 

/* NOTE: PORTA is a register that holds the information describing the state of each
   PORT A pin. If the pin is reading a "high input" then the value is 1 and,
   alternatively, if the pin is reading a "low input" then value is 0.  If pins RA2,
   RA3 and RA4 are ON, or equal to 1, then the binary value of PORTA register would be
   00000000 00011100 which is equal to decimal 28. However, we want it to equal 7 so
   it's necessary to shift the binary number by 2 bits to the right.  Shifting it by 2
   bits to the right we now have 00000000 00000111 which is the binary equivalent of 7.
   Aside - the bitwise operation ">>" is from C-programming and means to "shift right"*/      
                
        switch (Input)          //Chooses the correct case based on the variable Input 
        {
            case 0 :
                LATB = 0 << 4;  //Shifts by 4 and displays '0000 0000' on LEDs 
                break;
            case 1:
                LATB = 1 << 4;  //Shifts by 4 and displays '0000 0001' on LEDs 
                break;
            case 2:
                LATB = 4 << 4;  //Shifts by 4 and displays '0000 0100' on LEDs 
                break;
            case 3 :
                LATB = 9 << 4;  //Shifts by 4 and displays '0000 1001' on LEDs 
                break;
            case 4 :
                LATB = 16 << 4; //Shifts by 4 and displays '0001 0000' on LEDs 
                break;
            case 5 :
                LATB = 25 << 4; //Shifts by 4 and displays '0001 1001' on LEDs 
                break;
            case 6 :
                LATB = 36 << 4; //Shifts by 4 and displays '0010 0100' on LEDs 
                break;
            case 7 :
                LATB = 49 << 4; //Shifts by 4 and displays '0011 0001' on LEDs 
                break;
        }
    }
}

