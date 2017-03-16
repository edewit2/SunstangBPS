/*===============================================================================
Program1-5.c

Two variable resistors connected to AN0 and AN1 will be used to vary the voltage
from 0Vdc to 3.3Vdc.  This program converts analog input 0 (AN0) and analog input
1 (AN1) of PORTA to digital values.  Then compares these values, and if AN0 > AN1
then the LED which is connected to pin RB4 on PORTB is set HIGH/ON.  If 
AN0 < AN1 then RB4 is set LOW/OFF.
===============================================================================*/

#include "p24HJ128GP502.h"  //Includes device library file
#include "adc.h"            //Includes ADC library file

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF)
_FOSC   (POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT   (FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR   (FPWRT_PWR1 & ALTI2C_ON);
_FICD   (ICS_PGD1 & JTAGEN_OFF);

//Function prototype
void ADC(void);

//Global variables
unsigned int Result1 = 0;       
unsigned int Result2 = 0;       

int main(void)
{
    TRISA = 0xFFFF;                 //Set all of PORTA pins as inputs  
    TRISB = 0;                      //Set all of PORTB pins as outputs

    while (1)                       //Infinite loop
    {
        ADC();                      //Call ADC for the latest analog values
        if (Result1 > Result2)      //Check if Result1 > Result2
            {
                LATBbits.LATB4 = 1; //Turn on LED connected to RB4
            }
                else if (Result1 < Result2) //Check if Result1 < Result2
            {
                LATBbits.LATB4 = 0; //Turn off LED connected to RB4
            }       
    }
}

/*********************************************************************************************************/
// Routine to perform Analog-to-Digital conversion
// For more information on PIC24H ADC Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.25/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
/*********************************************************************************************************/
void ADC(void)
{
                //12 bit sampling
                //Samples AN0
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
    AD1CON1bits.ADON = 1;
    SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN0);
    while (AD1CON1bits.DONE == 0);
    Result1 = ReadADC1(0);
    AD1CON1bits.ADON = 0;
    
                //Samples AN1
    OpenADC1(ADC_MODULE_OFF & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_MANUAL & ADC_AUTO_SAMPLING_ON,
            ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_ALT_INPUT_OFF,
            ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC,
            ADC_DMA_BUF_LOC_1,
            ENABLE_AN1_ANA,
            0,
            0,
            0);
    AD1CON1bits.ADON = 1;
    SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN1);
    ConvertADC1();
    while (!AD1CON1bits.DONE);
    Result2 = ReadADC1(0);
    AD1CON1bits.ADON = 0;
}