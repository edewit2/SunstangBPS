/*================================================================================================================
 Program4-Controller.c; (Note: Program for the Controller PIC24H microcontroller)
 
 A variable resistor connected to AN0 will be used to vary a voltage from 0Vdc to
 3.3Vdc.  The ADC results are stored into a variable (ONTime) which will then be 
 added to an array (SendDataArray).  This array will utilize the UART1 hardware
 module to send this data through the TX pin to a HC-O5 Bluetooth Module.  Another
 HC-05 Bluetooth Module which is paired will be connected to the Robot PIC24H
 microcontroller. The Controller PIC24H microcontroller will also utilize the UART1
 module to receive data through the RX pin connected to the HC-05 Bluetooth Module
 and store this data into an array (ReceiveDataArray). A value from this array is stored
 into a variable (Photocell) and is used to adjust Timer3's Interrupt which will
 control the pulse rate of an LED connected to RB5.

 Note: Communications is setup for 115200 Baud, 8 Data Bits, 1 Stop Bit and No Parity Bit

 For information on PIC24H Compiler Peripheral Libraries refer to link below:
 file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/16-bit%20Peripheral%20Libraries.htm
=================================================================================================================*/
#include "p24HJ128GP502.h"  //Include device library file
#include "timer.h"          //Include TIMER library file
#include "uart.h"           //Include UART library file
#include "adc.h"            //Include ADC library file

#define BUFF_SIZE 32

//Configuration Bits
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC	(POSCMD_NONE & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSDCMD);
_FWDT	(FWDTEN_OFF & WINDIS_OFF & WDTPRE_PR32 & WDTPOST_PS1);
_FPOR	(FPWRT_PWR1 & ALTI2C_ON);
_FICD	(ICS_PGD1 & JTAGEN_OFF);

//Function prototypes
void InitIO (void);
void InitTimer (void);					
void InitUART(void);
void ADC (void);			
void ProcessData(void);
void SendData(void);

//Global variables
unsigned int Photocell = 0;     //Value received from UART1 to control the pulsing of an LED connected to RB5 
unsigned int ONTime = 0;        //Variable sent to the Robot PIC24H microcontroller via HC-05 Bluetooth Module			

unsigned char SendDataArray[BUFF_SIZE];     //Array to store the data to be transmitted through UART1 TX pin
unsigned char ReceiveDataArray[BUFF_SIZE];  //Array to store the data received from UART1 RX pin
unsigned char ReceivedChar = 0;             //Variable used to store the last byte received from UART1 RX pin
int i;                                      //Variable used to index through arrays
int uartcount = 0;                          //The array index currently receiving data from the UART RX pin

int main (void)
{
	InitIO();               //Call InitIO which configures the input and output pins
	InitTimer();            //Call InitTimer which configures Timer1 for controlling LED Pulsing
	InitUART();             //Call InitUART which configures the UART1 hardware module for communications
                            //with the HC-05 Bluetooth Module
	
	for (i=0; i<BUFF_SIZE; i++) SendDataArray[i] = 0;   //Initialize the array of chars to zero
	SendDataArray[0] = 's';                             //Set first element as 's' for data synchronization
                                                        //and for frame error checking

	while (1)               //Infinite loop
	{
		ADC();              //Call ADC which configures and reads analog inputs 0 and 1 (AN0 and AN1)
		ProcessData();      //Call ProcessData to update variables for UART1 Communications
		SendData();         //Call SendData to send data through the UART1 TX pin to HC-05 Bluetooth Module
	}
}
/*****************************************************************************************************************/
void InitIO (void)
{
	TRISAbits.TRISA0 = 1;	//Set RA0 (AN0) as input
	TRISBbits.TRISB5 = 0;	//Set RB5 as output for LED

	//RP8 TO U1RX           //Set the RP8 pin to UART1 RX pin
	RPINR18bits.U1RXR = 8;	//See TABLE 11-1: SELECTABLE INPUT SOURCES (MAPS INPUT TO FUNCTION),Page 136
                            //and REGISTER 11-8: RPINR18: PERIPHERAL PIN SELECT INPUT REGISTER 18,Page 146

    CNPU2bits.CN22PUE = 1;	//Enable weak pull-up of Receive pin CN22 (Corresponds to RP8)
                            //This is needed for v1.06 Bluetooth boards to pull up the receive line

	//RP9 TO U1TX           //Set the RP9 pin to UART1 TX pin
	RPOR4bits.RP9R = 3;     //See TABLE 11-2: OUTPUT SELECTION FOR REMAPPABLE PIN (RPn), Page 137
                            //and REGISTER 11-19: RPOR4: PERIPHERAL PIN SELECT OUTPUT REGISTERS 4, Page 154
}
/*****************************************************************************************************************/
void InitTimer(void)
// For more information on PIC24H Timers Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_Timers_Help.htm
{                                                   //Prescaler = 1:8 and Period = 0xFFFF					
	OpenTimer3 (T3_ON & T3_IDLE_STOP & T3_GATE_OFF & T3_PS_1_8 & T3_SOURCE_INT, 0xFFFF);							
	ConfigIntTimer3 (T3_INT_PRIOR_1 & T3_INT_ON);   //Set interrupt priority and Turn Timer3's interrupt on
}
/*****************************************************************************************************************/
void InitUART(void)
// For more information on PIC24H UART Peripheral Module Library Help refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_UART_Help.htm
{
 	IEC0bits.U1TXIE = 0; 		//Disable UART1 TX Interrupt
    IFS0bits.U1RXIF = 0;		//Clear the Receive Interrupt Flag

	U1MODEbits.STSEL = 0; 		//1 Stop it
	U1MODEbits.PDSEL = 0;	 	//8-bit data, no parity 
	U1MODEbits.BRGH = 0;		//16x baud clock, Standard mode
	U1MODEbits.URXINV = 0;		//Idle State 1 for RX pin
	U1MODEbits.ABAUD = 0;		//Auto-Baud Disabled
	U1MODEbits.RTSMD = 1;		//Simplex Mode, no flow control
	U1MODEbits.UARTEN = 1; 		//Enable UART1
	U1STAbits.UTXISEL0 = 0; 	//Interrupt after one TX character is transmitted
	U1STAbits.UTXISEL1 = 0; 	//Interrupt after one TX character is transmitted
	U1STAbits.UTXEN = 1; 		//Enable UART1 to control TX pin
	U1BRG = 1;                  //BAUD Rate Setting for 115200

	IEC0bits.U1RXIE = 1;		//Enable UART1 RX interrupt
}
/*****************************************************************************************************************/
void ADC (void)
// For more information on PIC24H ADC Peripheral Module Library refer to link below:
// file:///C:/Program%20Files%20%28x86%29/Microchip/xc16/v1.26/docs/periph_libs/dsPIC33F_PIC24H_dsPIC33E_PIC24E_ADC_Library_Help.htm
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
	ONTime = ReadADC1(0);           //ONTime = converted results
	AD1CON1bits.ADON = 0;           //Turn off ADC hardware module
}
/*****************************************************************************************************************/
void ProcessData(void)
{
	SendDataArray[1] = (ONTime >> 8);   //Populate the array one byte at a time
	SendDataArray[2] = ONTime;

	Photocell = (ReceiveDataArray[1] << 8) + ReceiveDataArray[2];   //Build integer from array of bytes
}
/*****************************************************************************************************************/
void SendData(void)
{
	for (i=0;i<BUFF_SIZE;i++)           //Index through the array from the start to the end of the array 
	{                                   //Note: The first byte is an ASCII Character "s" to denote the start
		WriteUART1(SendDataArray[i]);	//Send one byte of the data array to UART Tx Pin
		while(BusyUART1());             //Wait while the UART1 is busy
	}
}
/*****************************************************************************************************************/
void __attribute__((interrupt, auto_psv)) _T3Interrupt(void)
{
	DisableIntT3;                       //Disable Timer3 Interrupt
	LATBbits.LATB5 = 1 ^ PORTBbits.RB5;
	WriteTimer3(Photocell*16);	
	IFS0bits.T3IF = 0;                  //Reset Timer3 interrupt flag
	EnableIntT3;                        //Enable Timer3 interrupt
}
/*****************************************************************************************************************/
// UART1 Receive Interrupt
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) 
{
	DisableIntU1RX;             //Disable the UART1 receive interrupt
	IFS0bits.U1RXIF = 0;        //Reset the UART1 receive interrupt flag
	ReceivedChar = U1RXREG;     //Store the latest received character

//Need to synchronize the data being received by looking for the 's' which denotes the start of the array
    if ((uartcount == 0) && (ReceivedChar == 's'))
    {
        ReceiveDataArray[uartcount] = ReceivedChar; //Store 's' into the 0 element of the array
        uartcount++;
    }
    else if (uartcount != 0)
//Data has been synchronized; update the array of data being received until buffer size has been reached
    {
        ReceiveDataArray[uartcount] = ReceivedChar; //Update array with the latest data byte being received
        uartcount++;                                //Increment array index for next byte being received
        if (uartcount==BUFF_SIZE) uartcount=0;      //All data in array has been received
    }
     EnableIntU1RX;             //Enable the UART1 receive interrupt
}