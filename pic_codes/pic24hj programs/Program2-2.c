/*====================================================================================
 Program 2-2.c
 One variable resistor connected to AN0 will be used to vary the voltage from 0Vdc to
 3.3Vdc. This program converts analog input 0 of PORTA (AN0) to a binary value. The
 result is used to set the Pulse Width Modulation (PWM) signal of the Output Compare 
 Module 1 (OC1). This PWM signal from pin RB14 is sent to a DC motor (VEX Motor 393)
 via the IRLD024 Power MOSFET.  The speed of the motor is determined by the PWM duty
 cycle (0 - 100%) which is a percentage of the supply voltage to motor.

 For information on PIC24H Compiler Peripheral Libraries refer to link below:
 file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/16-bit%20Peripheral%20Libraries.htm
====================================================================================*/

#include "p24HJ128GP502.h"  //Include device library file
#include "timer.h"          //Includes Timer library file
#include "adc.h"            //Includes ADC library file
#include "outcompare.h"     //Includes Output Compare (PWM) library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC	(POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT	(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR	(FPWRT_PWR1 & ALTI2C_ON);
_FICD	(ICS_PGD1 & JTAGEN_OFF);

//Function prototypes
void InitIO(void);					
void InitPWM(void);					
void ADC(void);						
void Drive(void);					

//Global variables
unsigned int PWM = 0;       //Variable used to store the ADC result which controls motor's speed						

int main(void)
{
	InitIO();               //Call InitIO which configures the input and output pins
	InitPWM();              //Call InitPWM which configures the Output Compare Module for PWM mode
	
	while (1)               //Infinite loop
	{
		ADC();              //Call ADC which reads AN0 and stores result in PWM
		Drive();            //Call Drive which uses PWM variable to set the Duty Cycle
	}
}
/*********************************************************************************************************/
void InitIO(void)
{
	TRISAbits.TRISA0 = 1;   //Set RA0 (AN0) as input
	TRISBbits.TRISB14 = 0;  //Set RB14 as output 
	RPOR7bits.RP14R = 18;   //Set RP14(RB14) as OC1 output which is used to control motor speed
}
/*********************************************************************************************************/
// For more information on PIC24H Output Compare Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_dsPIC33E_PIC24H_PIC24E_OC_Library_Help.htm
//
// For more information on PIC24H Timers Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
/*********************************************************************************************************/
void InitPWM(void)
{			
                            //Timer2 is the clock source for OC1
                            //Configure PWM mode on OC1
	OpenOC1(OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE, 1, 1);
                            //Prescaler = 1:1
                            //Period = 0x0FFF
	OpenTimer2(T2_ON & T2_IDLE_CON & T2_GATE_OFF & T2_PS_1_1 & T2_32BIT_MODE_OFF & T2_SOURCE_INT, 0x0FFF);
}
/*********************************************************************************************************/
// For more information on PIC24H ADC Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.26/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
/*********************************************************************************************************/
void ADC (void)
{
									//12-bit sampling
									//Use dedicated ADC RC oscillator
									//Automatically start new conversion after previous
									//Use Avdd and Avss as reference levels
	OpenADC1( 	ADC_MODULE_OFF & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON,
				ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_ALT_INPUT_OFF,
				ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC,
				ADC_DMA_BUF_LOC_1,
				ENABLE_AN0_ANA,
				0,
				0,
				0);
									//Select AN0
	SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN0);
	AD1CON1bits.ADON = 1;			//Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	PWM = ReadADC1(0);				//Read conversion results and store in PWM
	AD1CON1bits.ADON = 0;			//Turn off ADC hardware module
}
/*********************************************************************************************************/
void Drive(void)
{	
	SetDCOC1PWM(PWM);       //Set PWM duty cycle which updates the signal coming from RB14 which is
}                           //used to adjust the average voltage which goes to the VEX DC motor
