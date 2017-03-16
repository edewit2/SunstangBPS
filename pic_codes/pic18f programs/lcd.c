/*
 This program configures an LCD and displays text. NOT WORKING!
 */

#include <stdio.h>
#include <stdlib.h>
#include <xlcd.h>
#include "SunstangBPS_temp.h"

void init_XLCD(void);           // initialize LCD display
void DelayFor18TCY(void);       // 18 cycles delay
void DelayPORXLCD(void);        // 15ms delay
void DelayXLCD(void);           // 5ms delay

void main(void) {
      
    OSCCON = 0x76;
    
    init_XLCD();                // Call the init_XLCD function to initialize the LCD
    putrsXLCD("SunstangBPS");   // Display "SunstangBPS"
    SetDDRamAddr(0x40);         // Shift cursor to beginning of second
    putrsXLCD("LCD Display");
    
    while(1)
    {
        
    }
    
}

// Initialize LCD Display
void init_XLCD(void)
{
    OpenXLCD(FOUR_BIT & LINES_5X7);     // Configure LCD in 4-bit data mode
    while(BusyXLCD());                  // Check if LCD controller is not busy
    WriteCmdXLCD(0x06);                 // Move cursor right, don't shift display
    WriteCmdXLCD(0x0C);                 // Turn display ON without cursor
    WriteCmdXLCD(0x01);                 // Clear screen
    return;
}

// 18 cycle delay
void DelayFor18TCY(void)
{
    Delay10TCY(2);      // Delay 20 TCY
    return;
}

void DelayPORXLCD(void)
{
    Delay1KTCYx(30);
    return;
}

void DelayXLCD(void)
{
    Delay1KTCYx(10);
    return;
}
