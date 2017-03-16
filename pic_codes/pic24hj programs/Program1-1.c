/*==================================================================================
 Program1-1.c

 This program is an introduction to pin inputs and outputs (I/O) on the PIC24H.
 When an input signal connected to pin RB12 is HIGH or LOW (1 or 0) then turn ON or 
 OFF an output which is connected to an LED on pin RB4.  The input may be changed by
 connecting a wire from pin RB12 to 3.3Vdc or to GROUND.  Another way would be to
 utilize a push button to change the voltage to the input pin.

===================================================================================*/
#include "p24HJ128GP502.h"          //Include device library file		

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC	(POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT	(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR	(FPWRT_PWR1 & ALTI2C_ON);
_FICD	(ICS_PGD1 & JTAGEN_OFF);

int main(void)
{
        AD1PCFGLbits.PCFG12 = 1;    //Refer to Digital IO Fix.c Supplementary C program
        TRISBbits.TRISB12 = 1;      //Set pin RB12 as an input
        TRISBbits.TRISB4 = 0;       //Set pin RB4 as an output

/******  TRISx  **********************************************************************
Using TRISx will allow the user to assign a single pin (or possibly all pins) as
either inputs or outputs.   0 = Output
                            1 = Input
TRISxbits.TRISx# = 1 will make that specific pin an input
TRISx = 1 will make only the first pin of the port as an input and the rest outputs
(i.e. 1 in decimal = 0000 0000 0000 0001 in a 16-bit binary number)

Note: x = either A or B
# = number corresponding to pin, see pic24 diagram
**************************************************************************************/

        while (1)                   //Program runs in infinite loop
        {

/******  PORTx  **********************************************************************
We can read from a pin by using PORTx. This only reads in either HIGH or LOW (ON/OFF)
To read in analog input or varying voltage then the analog input pins must be used.

PORTxbits.Rx# reads from a specific pin
PORTx reads from all pins on the port

Note: x = either A or B
# = number corresponding to pin, see pic24 diagram
*************************************************************************************/

/******  LATx  ***********************************************************************
We can control an output to a pin by using LATx. In this case, we are turning an LED
ON and OFF, so we output HIGH or LOW (1 or 0)

LATxbits.LATx# outputs to specific pin
LATx outputs to all pins on the port

Note: x = either A or B
# = number corresponding to pin, see pic24 diagram
*************************************************************************************/

//Check if the button is pressed, if so turn on LED, if not turn off LED
                if (PORTBbits.RB12)
                        LATBbits.LATB4 = 1;
                else
                        LATBbits.LATB4 = 0;
        }
}