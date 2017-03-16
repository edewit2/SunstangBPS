/*==================================================================================
 Program1-2+Interrupts.c

 This program loops a specified number of times by using a count-up scheme.
 The loop is controlled by the value stored in the user variable "Count", which
 is incremented by 1 each iteration.  The binary value of "Count" is displayed
 on PORTB.  When "Count" = 7 then the loop is finished and the program will
 reset "Count" = 0 and run again in an infinite loop.

 The timing of Count is controlled by a more efficient method (Timer interrupt).
==================================================================================*/

#include "p24HJ128GP502.h"      //Includes device library file
#include "timer.h"              //Include timer Library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC   (POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT   (FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR   (FPWRT_PWR1 & ALTI2C_ON);
_FICD   (ICS_PGD1 & JTAGEN_OFF);

//Function prototype
void InitTimer (void);

//Global Variable
unsigned int Count = 0;         //Initialize Count to zero, used to increment LEDs
unsigned int InterruptCount = 0;//Initialize to zero, used for timer interrupt events

int main(void)
{
    InitTimer();            //Call Routine InitTimer to initialize the Timer interrupt hardware
    TRISB = 0;              //Set all PORTB pins as outputs

    while (1)               //The program runs forever in an infinite loop
    {
        LATB = Count << 4;  //Shift the bits and display the value of Count on PORTB.
                            //The first four PORTB pins are skipped in order to display
                            //Count properly on the LEDs (based on Assignment #1 circuit)
    }
}

/*********************************************************************************************************/
// Routine to initialize and configure timer1 and interrupt hardware
// For more information on PIC24H Timers Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.25/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
/*********************************************************************************************************/

void InitTimer(void)
{
        //It's possible to do the equation by hand, or with the PIC timer calculator to obtain proper values

        //Prescaler = 1:8
        //Period = 0xFFFF (hexadecimal) = 65535 (decimal), this is the PR value in pic timer calculator program

        OpenTimer1 (T1_ON & T1_PS_1_8 & T1_SOURCE_INT & T1_GATE_OFF & T1_IDLE_STOP, 0xFFFF);

        //Turn Timer1 interrupt on
        ConfigIntTimer1 (T1_INT_PRIOR_7 & T1_INT_ON);
        
        //Load value to setup the timer1 interrupt with a 125ms delay 
        WriteTimer1(7957);
}

/*********************************************************************************************************/
// This interrupt is activated every 150 milliseconds (determined by the Period and TMR value in InitTimer()
// routine, and this timing is used to increment the Count variable.  Need to enter the timer interrupt twice
// before incrementing Count as we want a 250ms delay but the timer interrupt is setup for 125ms delay.
// For more information on PIC24H Interrupts, see the MPLAB® XC16 C Compiler User's Guide - Chapter 14. Interrupts
/*********************************************************************************************************/

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)
{
	DisableIntT1;           //Disable Timer1 Interrupt
    InterruptCount++;       //Increments Count1 by 1 in order to count multiple 125ms delays

    if (InterruptCount==2)  //Interrupt is configured for 125ms therefore we need to count the number
    {                       //of times we enter the timer interrupt (2 times for a 250ms delay)
        InterruptCount=0;   //Reset Count1 in order to setup for another 250ms delay
        Count++;            //Increments Count by 1
        if (Count > 7)      //Check if Count needs to be reset to 0
            Count = 0;      //Reset Count equal to 0 for the next iteration of Count
    }
    WriteTimer1(7957);      //Reload value to setup the timer interrupt with a 125ms delay
	IFS0bits.T1IF = 0;      //Reset Timer1 interrupt flag
	EnableIntT1;            //Enable Timer1 interrupt
}

