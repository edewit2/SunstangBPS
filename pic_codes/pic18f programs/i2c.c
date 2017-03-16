/*
 NOT WORKING
 */

#include <stdio.h>
#include <stdlib.h>
#include "SunstangBPS_temp.h"

// Declarations
#define I2C_ADDR 0x30   // 0b00110000 read address
                        // 0b00110001 write address

void init_I2C(void);
void interrupt ISR(void);

unsigned volatile char data_in;                 // I2C data in
unsigned volatile char data_out = 0b00001111;   // I2C data out
unsigned volatile char address;                 // Temp
unsigned volatile char clear;                   // Temp


void main(void) {

    ADCON1 = 0b00001111;          // Set ANx to digital I/O
    OSCCON = 0x76;          // 8MHz internal oscillator
    
    TRISB = 0;            // Set PORTB output
    LATB = 0x01;
    __delay_ms(1000);
    
    init_I2C();
    
    PEIE = 1;    // Peripheral interrupt enable (INTCON)

    while(1)
    {
        Delay1KTCYx(30);     // Delay = 30000 cycles
    }
    
}

void init_I2C(void)
{    
    TRISCbits.RC3 = 1;           // Set SDA pin as input
    TRISCbits.RC4 = 1;           // SCL SCL pin as input
    
    SSPSTAT = 0b10000000;   // Disable slew rate control (100kHz)
    SSPCON1 = 0b00110110;   // Slave-mode 7-bit address, Release clock, MSSP enable
    SSPCON2 = 0b00000001;   // Enable clock stretching
    SSPADD = I2C_ADDR;    
    
    GIE = 1;     // General interrupt enable (INTCON)
    PEIE = 1;    // Peripheral interrupt enable (INTCON)
    SSPIE = 1;     // Enable MSSP interrupt (PIE1)
    SSPIF = 0;     // Clear interrupt flag (PIR1)
}

void interrupt ISR(void)
{
    LATB = 0x02;
    if(SSPIF == 1)      // MSSP interrupt routine
    {
        //LATB = 0x03;
        
        SSPIF = 0;      // Clear MSSP interrupt flag
        CKP = 0;        // Hold clock low

        if(SSPOV == 1 || WCOL == 1)
        {
            clear = SSPBUF; // Read the previous value to clear the buffer
            SSPOV = 0; // Clear the overflow flag
            WCOL = 0;  // Clear the collision bit
            CKP = 1;
        }
        if(R_W == 1 && D_A == 0 && BF == 1)      // Read instruction + address
        {
            address = SSPBUF;         // Read address from buffer
            BF = 0;                   // Clear buffer flag bit
            CKP = 1;                  // Release clock
        }
        if(R_W == 1 && D_A == 1 && BF == 1)      // Read instruction + data
        {
            data_in = SSPBUF;         // Read data from buffer
            BF = 0;
            CKP = 1;
        }
        if(R_W == 0 && D_A == 0 && BF == 1)      // Write instruction + address
        {
            address = SSPBUF;
            BF = 0;
            CKP = 1;
        }
        if(R_W == 0 && D_A == 1 && BF == 1)      // Write instruction + data
        {
            SSPBUF = data_out;        // Write data to buffer
            BF = 0;
            CKP = 1;
        }

 
   }
}