/* 
 * File:   SunstangBPS_temp.c
 * Author: edewit2
 *
 * Created on December 24, 2016, 3:18 PM
 */

/*
 This program configures an LCD and displays text.
 */

#include <stdio.h>
#include <stdlib.h>
#include <adc.h>
#include <timers.h>
#include "SunstangBPS_temp.h"

void init_IO(void);             // initialize IO pins
void init_ADC(void);            // initialize A/D converter
void init_Timer1(void);         // initialize Timer1
void interrupt ISR(void);       // Interrupt serive routine

unsigned int ADCResult = 0;
unsigned int TMRCount = 0;
unsigned int MUXCount = 0;
float voltage = 0;

void main(void) {
    
    OSCCON = 0x76;             // Configure to use Internal 8MHz oscillator
    
    init_IO();
    init_ADC();
    init_Timer1();

    while(1)
    {    
        LATB = MUXCount;

        if (voltage < 2)
        {
            LATBbits.LATB4 = 1;
        }
    }
    
}

// Initialize IO pins
void init_IO(void)
{
    TRISA = 0xFF;   // Set PORTA bits as input
    TRISB = 0;      // Set PORTB bits as output
    
    return;
}

void init_Timer1(void)
{
    OpenTimer1( TIMER_INT_ON &      // Interrupt enabled
                T1_8BIT_RW &        // Set timer1 as two 8-bit registers
                T1_SOURCE_INT &     // Choose internal clock source (TOSC))
                T1_PS_1_8 &         // Pre-scale value 1:8
                T1_OSC1EN_OFF &     // Disable Timer1 oscillator
                T1_SYNC_EXT_OFF     // Do not sync external clock input
               );
    
    /* Timer increment frequency = (Fosc/4)/8 = (8MHz/4)/8 = 0.25MHz
     * Generate interrupt every 125ms
     * Register value = 125ms*0.25MHz = 31250
     * Timer1 starting value = 0xFFFF - 31250 = 34285 = 0x85ED
    */
    
    WriteTimer1(0x85ED);        // Load value to setup TMR1 with 125ms delay
    
    GIE = 1;           // Global interrupt enable (INTCON1 register)
    PEIE = 1;          // Peripheral interrupt enable (INTCON1 register)
    TMR1IE = 1;        // Enables the TMR1 overflow interrupt (PIE1 register)
    TMR1IF = 0;        // Clear TMR1 overflow Flag bit (PIR1 register)
    
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
            ADC_CH0 & ADC_INT_OFF & ADC_REF_VDD_VSS,
            ADC_1ANA);
    
    return;
}

void interrupt ISR(void)
{
    if (TMR1IF == 1)       // TMR1 interrupt flag
    {         
        ADCResult = 0;
        // --- sample and convert ---
        ConvertADC();
        while(BusyADC());
        ADCResult = ReadADC();
        voltage = (ADCResult*5.0)/1024;
        
        TMRCount++;
        if (TMRCount == 4)          // Increment 4 times for 500ms delay
        {
            TMRCount = 0;           // Reset for another 500ms delay
            MUXCount++;
            if (MUXCount > 15)
                MUXCount = 0;
        }
        WriteTimer1(0x85ED);        // Reload value to setup TMR1 with 125ms delay
        TMR1IF = 0;        // Reset Timer1 interrupt flag
    }
    
    return;
}
