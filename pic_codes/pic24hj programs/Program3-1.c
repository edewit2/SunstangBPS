/*====================================================================================
 Program3-1.c;
 This program reads two variable resistors which are connected to AN0 and AN1.  The 
 results are used to adjust the PWM signal coming from OC1 and OC2 output pins which
 are configured for PWM mode.  These signals are used to control the speed of two
 VEX DC motors via the DC Motor Driver Boards #s420. The supply voltage to the DC 
 motor driver board should be 8.4Vdc.  The DC Motor Driver needs 100% duty cycle for
 OFF and 0% Duty cycle for full ON therefore we need to condition the signal by
 inverting it.  Also, the ADC value is only 12 bit but the PWM value needs to be
 16 bit therefore we multiply the value by 16 to scale it appropriately.
======================================================================================
 For information on PIC24H Compiler Peripheral Libraries refer to link below:
 file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/16-bit%20Peripheral%20Libraries.htm
====================================================================================*/

#include "p24HJ128GP502.h"	//Include device library file
#include "timer.h"          //Include Timer library file
#include "adc.h"            //Include ADC library file
#include "outcompare.h"		//Include Output Compare (PWM) library file

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
void CondPWM(void);			
void Drive(void);			

//Global variables
unsigned int PWM1 = 65535;	//Initialize PWM1, used to store value of PWM Signal to motor 1 			
unsigned int PWM2 = 65535;	//Initialize PWM2, used to store value of PWM Signal to motor 2
                            //Note the value of 65535 represents 100% Duty Cycle = motor off 	
int main(void)
{
	InitIO();		//Call InitIO function
	InitPWM();		//Call InitPWM function
	
	while (1)		//Infinite loop
	{
		ADC();		//Call ADC function
		CondPWM();	//Call CondPWM function, used to adjust the PWM signals
		Drive();	//Call Drive function, used to update PWM Hardware with new PWM signals
	}
}

/*********************************************************************************************************/
void InitIO(void)
{
	TRISAbits.TRISA0 = 1;	//Set RA0 as input (variable resistor connected to AN0)
	TRISAbits.TRISA1 = 1;	//Set RA1 as input (variable resistor connected to AN1)

	TRISBbits.TRISB14 = 0;	//Set RB14 as output (PWM Signal for Motor 1)
	TRISBbits.TRISB13 = 0;	//Set RB13 as output (Direction Signal for Motor 1)
	TRISBbits.TRISB12 = 0;	//Set RB12 as output (PWM Signal for Motor 2)	
	TRISBbits.TRISB11 = 0;	//Set RB11 as output (Direction Signal for Motor 2)
	TRISBbits.TRISB10 = 0;	//Set RB10 as output (Disable Signal for Motors 1 and 2)

//Configure the appropriate Peripheral Pin Select bits to map OC1 and OC2 output pins to RB14 and RB12
//For more information refer to pages 137 and 155 of the PIC24H Data Sheet
    
    RPOR7bits.RP14R = 18;	//Set RP14 as OC1 output on pin RB14
	RPOR6bits.RP12R = 19;   //Set RP12 as OC2 output on pin RB12

	LATBbits.LATB13 = 0;    //Set the direction of motor 1 to be Forward
	LATBbits.LATB11 = 0;    //Set the direction of motor 2 to be Forward
                            //(Note: May need to swap motor leads to have motor spin forward)
	LATBbits.LATB10 = 1;    //Enable motors to run
}
/*********************************************************************************************************/
//For more information on PIC24H Output Compare Peripheral Module Library refer to link below:
//file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_dsPIC33E_PIC24H_PIC24E_OC_Library_Help.htm
//
//For more information on PIC24H Timers Peripheral Module Library refer to link below:
//file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
/*********************************************************************************************************/
void InitPWM(void)
{
	DisableIntT2;		//Disable Timer2 Interrupt
	DisableIntOC1;		//Disable OC1 Interrupt
	DisableIntOC2;		//Disable OC2 Interrupt
                        //Timer2 is the clock source for OC1 and OC2
                        //Configure PWM mode for OC1 and OC2
	OpenOC1(OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE, 1, 1);
	OpenOC2(OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE, 1, 1);							
                        //Prescaler = 1:1 and Period = 0xFFFF
	OpenTimer2(T2_ON & T2_IDLE_CON & T2_GATE_OFF & T2_PS_1_1 & T2_32BIT_MODE_OFF & T2_SOURCE_INT, 0xFFFF);
}
/*********************************************************************************************************/
//For more information on PIC24H ADC Peripheral Module Library refer to link below:
//file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.26/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
//********************************************************************************************************/
void ADC(void)
{                                   //12-bit sampling
                                    //Use dedicated ADC RC oscillator
                                    //Automatically start new conversion after previous
                                    //Use Avdd and Avss as reference levels
	OpenADC1(ADC_MODULE_OFF & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON,
			ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_ALT_INPUT_OFF,
			ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC,
			ADC_DMA_BUF_LOC_1,
			ENABLE_AN0_ANA,
			0,
			0,
			0);
                                    //Select AN0
	SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN0);
	AD1CON1bits.ADON = 1;           //Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	PWM1 = ReadADC1(0);             //Read conversion results and store in PWM1
	AD1CON1bits.ADON = 0;           //Turn off ADC hardware module

	OpenADC1(ADC_MODULE_OFF & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON,
			ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_ALT_INPUT_OFF,
			ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC,
			ADC_DMA_BUF_LOC_1,
			ENABLE_AN1_ANA,
			0,
			0,
			0);
                                    //Select AN1
	SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN1);
	AD1CON1bits.ADON = 1;           //Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	PWM2 = ReadADC1(0);             //Read conversion results and store in PWM2
	AD1CON1bits.ADON = 0;           //Turn off ADC hardware module
}
/*********************************************************************************************************/
void CondPWM(void)
{										
	PWM1 = ~(PWM1 *16);	//Invert, multiply by 16 to scale a 12 bit value to 16 bit memory location
	PWM2 = ~(PWM2 *16);	//Invert, multiply by 16 to scale a 12 bit value to 16 bit memory location
}
/*********************************************************************************************************/
void Drive(void)
{	
	SetDCOC1PWM(PWM1);	//Set duty cycle PWM1
	SetDCOC2PWM(PWM2);	//Set duty cycle PWM2
}
