/*====================================================================================
 Program 2-1.c
 Two variable resistors connected to AN0 and AN1 will be used to vary the voltage
 from 0Vdc to 3.3Vdc.  This program converts analog input 0 (AN0) and analog input
 1 (AN1) of PORTA to digital values. The ADC results are then used to adjust the duration
 of Timer1's interrupt which, in turn, sets the ON and OFF Time (from AN0 and AN1,
 respectively) of the square wave signal sent to the RC Servo Motor connected to RB6.
 The ON Time, or Pulse Width, will determine the angular position of the RC Servo Motor 
 assuming the OFF Time is set properly.

 For information on PIC24H Compiler Peripheral Libraries refer to link below:
 file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/16-bit%20Peripheral%20Libraries.htm
====================================================================================*/

#include "p24HJ128GP502.h"  //Include device library file
#include "adc.h"            //Include ADC library file
#include "timer.h"          //Include Timer library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC	(POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT	(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR	(FPWRT_PWR1 & ALTI2C_ON);
_FICD	(ICS_PGD1 & JTAGEN_OFF);

//Function prototype
void InitIO (void);				
void ADC (void);					
void InitTimer (void);				

//Global variables
unsigned int ONTime = 0;    //Variable used to control the RC Servo motor's Position			
unsigned int OFFTime = 0;   //Main variable used to control the period of the square wave	
unsigned int TmrState = 0;  //Variable used to store whether square wave is ON or OFF 		
unsigned int TmrVal = 0;    //Variable to store the value used to setup Timer1 which 
                            //controls the ON and OFF time by utilizing Timer1's Interrupt			
int main (void)
{
	InitIO();               //Call InitIO which configures the input and output pins
	InitTimer();            //Call InitTimer which configures the timer and its interrupt

	while (1)               //Infinite loop
	{
		ADC();              //Call ADC which configures and reads analog inputs 0 and 1 (AN0 and AN1)	
    }
}

/*********************************************************************************************************/
void InitIO (void)
{
	TRISAbits.TRISA0 = 1;   //Set RA0 (AN0) as input
	TRISAbits.TRISA1 = 1;   //Set RA1 (AN1) as input
	TRISBbits.TRISB6 = 0;   //Set RB6 as output which is used to generate the square wave signal				
}

/*********************************************************************************************************/
// For more information on PIC24H Timers Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
//********************************************************************************************************/
void InitTimer(void)
{                           //Prescaler = 1:1
                            //Period = 0x0FFF
	OpenTimer1 (T1_ON & T1_PS_1_1 & T1_SYNC_EXT_OFF & T1_SOURCE_INT & T1_GATE_OFF & T1_IDLE_STOP, 0x0FFF);
                            //Turn Timer1 interrupt ON
	ConfigIntTimer1 (T1_INT_PRIOR_7 & T1_INT_ON);
}
/*********************************************************************************************************/
// For more information on PIC24H ADC Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.26/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
//********************************************************************************************************/
void ADC (void)
{                                   //12-bit sampling
                                    //Use dedicated ADC RC oscillator
                                    //Automatically start new conversion after previous
                                    //Use AVdd and AVss as reference levels
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
	AD1CON1bits.ADON = 1;			//Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	ONTime = ReadADC1(0);			//ONTime = converted results
	AD1CON1bits.ADON = 0;			//Turn off ADC hardware module
    
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
	AD1CON1bits.ADON = 1;			//Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	OFFTime = ReadADC1(0);			//OFFTime = converted results
	AD1CON1bits.ADON = 0;			//Turn off ADC hardware module
}
/*********************************************************************************************************/
// For more information on PIC24H Interrupts, see the MPLAB® XC16 C Compiler User's Guide
// - Chapter 14. Interrupts
// For more information on PIC24H Timers Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
/*********************************************************************************************************/
void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)	
{
	DisableIntT1;               //Disable Timer1 interrupt 

// This IF statement will constantly switch in order to generate the square wave signal (ONTime and OFFTime)
	if (TmrState == 0)          //If signal is low (OFF)
	{
		LATBbits.LATB6 = 1;     //Turn ON Output to set high signal for RB6
		T1CONbits.TCKPS = 1;    //Change prescaler to 1:8
		TmrVal = ONTime;        //Set TmrVal = ONTime
		TmrState = 1;           //Set signal state to be ON for next interrupt
	}
	else if (TmrState == 1)     //If signal is HIGH (ON)
	{
		LATBbits.LATB6 = 0;     //Turn OFF Output to set LOW signal for RB6
		TmrVal = OFFTime;       //Set TmrVal = OFFTime
		T1CONbits.TCKPS = 2;    //Change prescaler to 1:64
		TmrState = 0;           //Set Timer state to be OFF for next interrupt in order to repeat again
	}	
	WriteTimer1(TmrVal);        //Setup Timer1 with the appropriate value to set the interrupt time
	IFS0bits.T1IF = 0;          //Reset Timer1 interrupt flag
	EnableIntT1;                //Enable Timer1 interrupt
}