/*==================================================================================
 Digital IO Fix.c
 This program will set the analog pins (ANx) to be used for digital inputs or
 outputs. The ANx pins are normally analog pins however, they can be configured as
 digital pins. In programs that use ADC conversion, the ANx pins that aren't used for
 ADC are configured as digital I/O. If no ADC is used in a program and you want to
 use ANx pins for digital I/O, then you need to write to the AD1PCFGL register to
 configure the ANx pins to be digital.  The uncommented lines of this program is 
 similar to Program1-1.c from Assignment #1.
===================================================================================*/
#include "p24HJ128GP502.h" 		//Include device library file

//Configuration Bits
//See "Section 25. Device Configuration - PIC24H Data Sheet.pdf"
//Section 25.2 DEVICE CONFIGURATION REGISTERS
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC	(POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT	(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR	(FPWRT_PWR1 & ALTI2C_ON);
_FICD	(ICS_PGD1 & JTAGEN_OFF);

void InitIO(void);				//InitIO function prototype

int main(void)
{
    InitIO();

	while (1)
	{
		LATBbits.LATB4 = PORTBbits.RB12; //Set Digital output at RB4 to the
	}                                    //value of Digital input at RB12
}
void InitIO(void)
{
	TRISBbits.TRISB12 = 1;      //Set pin RB12 as an input
    TRISBbits.TRISB4 = 0;       //Set pin RB4 as an output

//	See Section 11.3 Configuring Analog Port Pins (page 134) and AD1PCFGL Register (page 230)
//  0 will configure a pin as analog and 1 will configure it as digital
//	AD1PCFGL = 0xFFFF;			//Turn all ANx pins to Digital I/O 
								//RA0-RA1, RB0-RB3, RB12-RB15
/* It should be noted that the register can be directly written to as before
 * However it can be done in hexadecimal or binary or even decimal
 * All of the following statements are identical and make RB12 a digital pin while leaving 
 * all other ANx pins analog.
 * AD1PCFGL = 4096;               //Writes to register in decimal
 * AD1PCFGL = 0x1000;             //Writes to register in hexadecimal
 * AD1PCFGL = 0b0001000000000000; //Writes to register in binary 
 * AD1PCFGLbits.PCFG12 = 1;       //Writes to register bit 12 directly ignoring the other pins 
*/
//	AD1PCFGLbits.PCFG0 = 1;		//Set AN0 (RA0) to Digital IO
//	AD1PCFGLbits.PCFG1 = 1;		//Set AN1 (RA1) to Digital IO

//  Note that AN2 and AN3 pins are not available in debug configuration
//	The PGEC1/PGED1 pins are used by the debugger
//  See "Section 25. Device Configuration - PIC24H FRM.pdf"
//	Section 25.3.7 In-Circuit Debugger
//	AD1PCFGLbits.PCFG2 = 1;		//Set AN2 (RB0) to Digital IO
//	AD1PCFGLbits.PCFG3 = 1;		//Set AN3 (RB1) to Digital IO

//	AD1PCFGLbits.PCFG4 = 1;		//Set AN4 (RB2) to Digital IO
//	AD1PCFGLbits.PCFG5 = 1;		//Set AN5 (RB3) to Digital IO   
//	AD1PCFGLbits.PCFG9 = 1;		//Set AN9 (RB15) to Digital IO
//	AD1PCFGLbits.PCFG10 = 1;	//Set AN10 (RB14) to Digital IO
//	AD1PCFGLbits.PCFG11 = 1;	//Set AN11 (RB13) to Digital IO
	AD1PCFGLbits.PCFG12 = 1;	//Set AN12 (RB12) to Digital IO  
}
