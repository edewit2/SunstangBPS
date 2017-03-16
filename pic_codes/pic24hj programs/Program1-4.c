/*===============================================================================
 Program1-4.c

 One variable resistor connected to AN0 will be used to vary the voltage from
 0Vdc to 3.3Vdc. This program converts analog input 0 of PORTA (AN0) to a binary
 value and displays this value on PORTB which are connected to eight LEDs. Note 
 that the program actually produces a 12 bit number but the 4 least significant 
 bits(LSB)on PORTB are not connected to LEDs.
 
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

unsigned int Result = 0;    /*Initialize Result to 0, this variable is used to
                              store the Analog to Digital Convertor value*/
int main(void)
{
    TRISA = 0xFFFF;         /*Set all of PORTA pins as inputs (0xFFFF is a hexadecimal
                              form of 65535 in decimal or 11111111 11111111 in binary*/
                              
    TRISB = 0;              //Set all of PORTB pins as outputs
        
    while (1)
    {
        ADC();              /*Call ADC function which uses an Analog-to-Digital
                              Convertor in hardware to read the voltage at the pin
                              and converts this value into a digital form*/

        LATB = Result;      //Display the result on PORTB which is connected to 8 LEDs

/*NOTE - that we are sampling with 12 bit resolution from the Analog-to-Digital Convertor,
  so we need 12 LEDs to properly show the result.  Notice we are not shifting by 4 this
  time. The least significant 4 bits of the result is not being displayed by our circuit.
  The ADC in this microcontroller can only be configured to sample 12 or 10 bits.
*/
    }
}


/*********************************************************************************************************/
// Routine to perform Analog-to-Digital conversion
// For more information on PIC24H ADC Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.25/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
/*********************************************************************************************************/
void ADC(void)
{
                // 12-bit sampling
                // Samples AN0
                // Uses dedicated ADC RC oscillator
                // Automatically start new conversion after previous
                // Use AVdd and AVss as reference levels
    OpenADC1(ADC_MODULE_OFF & ADC_AD12B_12BIT & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON,
                ADC_VREF_AVDD_AVSS & ADC_SCAN_OFF & ADC_ALT_INPUT_OFF,
                ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC,
                ADC_DMA_BUF_LOC_1,
                ENABLE_AN0_ANA,
                0,
                0,
                0);
    AD1CON1bits.ADON = 1;                   //Turn on ADC hardware module
    SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN0);
    while (AD1CON1bits.DONE == 0);          //Wait until conversion is done
    Result = ReadADC1(0);                   //Store the result of the ADC 
    AD1CON1bits.ADON = 0;                   //Turn off ADC hardware module
}
