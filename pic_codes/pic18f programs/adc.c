/* 
 * File:   SunstangBPS_temp.c
 * Author: edewit2
 *
 * Created on January 2, 2016, 3:00PM
 */

/*
 This program configures the ADC using the interruupt to sample an input on AN0
 and displays the result on PORTB
 */

#include <stdio.h>
#include <stdlib.h>
#include <adc.h>
#include "SunstangBPS_temp.h"

void init_IO(void);             // initialize IO pins
void init_ADC(void);            // initialize A/D converter

unsigned int ADCResult = 0;
float voltage = 0;


void main(void) {
    
    OSCCON = 0x76;             // Configure to use Internal 8MHz oscillator
    
    init_IO();
    init_ADC();                // Call the init_XLCD function to initialize the LCD

    while(1)
    {    
        ADCResult = 0;
        //--- sample and convert ---
        ConvertADC();           // Start conversion
        while(BusyADC());       // Wait until conversion is done
        ADCResult = ReadADC();  // Read the converted value
        voltage = (ADCResult*5.0)/1024; // Convert ADC count into voltage
        
        LATB = ADCResult;
    }
    
}

// Initialize IO pins
void init_IO(void)
{
    TRISA = 0xFF;   // Set PORTA bits as input
    TRISB = 0;      // Set PORTB bits as output
    
    return;
}

// Initialize A/D converter
void init_ADC(void)
{
    /* ADC configured for:
     * FOSC/2 as conversion clock
     * Result is right justified
     * Acquisition time of 2 TAD
     * Channel 1 for sampling
     * ADC interrupt on
     * ADC reference voltage from VDD & VSS
    */
    
    OpenADC(ADC_FOSC_2 & ADC_RIGHT_JUST & ADC_2_TAD,
            ADC_CH0 & ADC_INT_ON & ADC_REF_VDD_VSS,
            ADC_1ANA);
    
    return;
}
