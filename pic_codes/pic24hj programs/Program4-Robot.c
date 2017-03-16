/*================================================================================================================
 Program4-Robot.c;	(Note: Program for the Robot PIC24H microcontroller) 

 A Photocell is connected to AN4 with the ADC results being stored into a variable
 (Photocell) which will be added to an array (SendDataArray). This array will
 utilize the UART1 hardware module to send this data through the TX pin to a HC-05
 Bluetooth Module. Another HC-05 Bluetooth Module which is paired will be connected to 
 the Controller PIC24H microcontroller. The Robot PIC24H microcontroller will also utilize
 the UART1 module to receive data through the RX pin connected to the HC-05 Bluetooth Module
 and store this data into an array (ReceiveDataArray). Values from this array will be
 stored into a variable (ONTime) and be used to adjust the duration of Timer1's interrupt.
 This in turn sets the ON Time of the square wave signal sent to the RC Servo Motor
 connected to RB6.  The ON Time, also referred to as Positive Width, will determine the
 angular position of the RC Servo Motor assuming the OFF Time is set properly. Also,
 included in this code is a SetServoLimits function which will limit the RC Servo Motor's
 position within its operating range (i.e. 0 to 180 degrees) by using define statements
 for the appropriate values for these limits (Note: these limit values may need to be
 adjusted for different RC Servo Motors).  The OFF time is also fixed at a value the
 represents ~19ms.

 Note: Communications is setup for 115200 Baud, 8 Data Bits, 1 Stop Bit and No Parity Bit

 For information on PIC24H Compiler Peripheral Libraries refer to link below:
 file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/16-bit%20Peripheral%20Libraries.htm
=================================================================================================================*/
#include "p24HJ128GP502.h"  //Include device library file
#include "timer.h"          //Include TIMER library file
#include "uart.h"           //Include UART library file
#include "adc.h"            //Include ADC library file

#define BUFF_SIZE 32
#define SERVO_MAX 3848
#define SERVO_MIN 3036

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
int SetServoLimits(int time);

//Global variables
unsigned int Photocell = 0;	//Variable sent to the Robot PIC24H microcontroller via HC-05 Bluetooth Module
unsigned int ONTime = 0;    //Variable used to control the RC Servo Motor's Position			
unsigned int OFFTime = 3000;//Variable used to control the period of the square wave	
unsigned int TmrState = 0;  //Variable used to store whether square wave is ON or OFF		
unsigned int TmrVal = 0;    //Variable used to store the value used to setup Timer1 which controls
                            //the ON and OFF time by utilizing Timer1's Interrupt
unsigned char SendDataArray[BUFF_SIZE];     //Array to store the data to be transmitted through UART1 TX pin
unsigned char ReceiveDataArray[BUFF_SIZE];  //Array to store the data received from UART1 RX pin
unsigned char ReceivedChar = 0;             //Variable used to store the last byte received from UART1 RX pin
int i;                      //Variable used to index through arrays
int uartcount = 0;          //The array index currently receiving data from the UART1 RX pin

int main (void)
{
	InitIO();           //Call InitIO which configures the input and output pins
	InitTimer();		//Call InitTimer which configures Timer1 for controlling the RC Servo Motor
	InitUART();         //Call InitUART which configures the UART hardware module for communications
                        //with the HC-05 Bluetooth Module
	
	for (i=0; i<BUFF_SIZE; i++) SendDataArray[i] = 0;   //Initialize the array of chars to zero
	SendDataArray[0] = 's';                             //Set first element as 's' for data synchronization
                                                        //and for framing error checking

	while (1)           //Infinite loop
	{
		ADC();          //Call ADC which configures and reads analog input 4 (AN4)
		ProcessData();	//Call ProcessData to update variables for UART1 Communications
		SendData(); 	//Call SendData to send data through the UART1 TX pin to HC-05 Bluetooth Module
	}
}
/*****************************************************************************************************************/
void InitIO (void)
{
	TRISBbits.TRISB2 = 1;	//Set RB2 as input for Photocell connected to AN4
	TRISBbits.TRISB6 = 0;	//Set RB6 as output for RC Servo Motor

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
{                                   //Prescaler = 1:1 and Period = 0x0FFF
	OpenTimer1 (T1_ON & T1_PS_1_1 & T1_SYNC_EXT_OFF & T1_SOURCE_INT & T1_GATE_OFF & T1_IDLE_STOP, 0x0FFF);						
	ConfigIntTimer1 (T1_INT_PRIOR_1 & T1_INT_ON);	//Set interrupt priority and turn Timer1 interrupt on
}
/*****************************************************************************************************************/
void InitUART(void)
// For more information on PIC24H UART Peripheral Module Library Help refer to link below:
// file:///C:/Program%20Files%20(x86)/Microchip/xc16/v1.26/docs/periph_libs/dsPIC30F_dsPIC33F_PIC24H_dsPIC33E_PIC24E_UART_Help.htm
{
	IEC0bits.U1TXIE = 0;            //Disable UART1 TX interrupt
    IFS0bits.U1RXIF = 0;            //Clear the Receive Interrupt Flag

	U1MODEbits.STSEL = 0;           //1 Stop it
	U1MODEbits.PDSEL = 0;           //8-bit data, no parity
	U1MODEbits.BRGH = 0;            //16x baud clock, Standard mode
	U1MODEbits.URXINV = 0;          //Idle State 1 for RX pin
	U1MODEbits.ABAUD = 0;           //Auto-Baud Disabled
	U1MODEbits.RTSMD = 1;           //Simplex Mode, no flow control
	U1MODEbits.UARTEN = 1;          //Enable UART1
	U1STAbits.UTXISEL0 = 0;         //Interrupt after one TX character is transmitted
	U1STAbits.UTXISEL1 = 0;         //Interrupt after one TX character is transmitted
	U1STAbits.UTXEN = 1;            //Enable UART1 to control TX pin
	U1BRG = 1;                      //BAUD Rate Setting for 115200

	IEC0bits.U1RXIE = 1;            //Enable UART1 RX interrupt
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
		ENABLE_AN4_ANA,
		0,		
		0,
		0);
                                    //Select AN4
	SetChanADC1(0, ADC_CH0_NEG_SAMPLEA_VREFN & ADC_CH0_POS_SAMPLEA_AN4);
	AD1CON1bits.ADON = 1;           //Turn on ADC hardware module
	while (AD1CON1bits.DONE == 0);	//Wait until conversion is done
	Photocell = ReadADC1(0);        //Photocell = converted results
	AD1CON1bits.ADON = 0;           //Turn off ADC hardware module
}
/*****************************************************************************************************************/
void ProcessData(void)
{	
	SendDataArray[1] = (Photocell >> 8);    //Populate the array one byte at a time
	SendDataArray[2] = Photocell;

	ONTime = (ReceiveDataArray[1] << 8) + ReceiveDataArray[2];  //Build integer from array of bytes

	ONTime = SetServoLimits(ONTime);        //Make sure the RC Servo Motor values are within its limits
}
/*****************************************************************************************************************/
int SetServoLimits(int time)        //Set Servo Limits
{			
	if (time > SERVO_MAX)
		return SERVO_MAX;
	else if (time < SERVO_MIN)
		return SERVO_MIN;
	else
		return time;
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
void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)	
{
	DisableIntT1;               //Disable Timer1 interrupt 

//This if statement will constantly switch in order to generate the square wave signal (ONTime and OFFTime)
	if (TmrState == 0)          //If signal is low (OFF)
	{
		LATBbits.LATB6 = 1;     //Set high signal for RB6
		T1CONbits.TCKPS = 1;	//Change prescaler to 1:8
        ONTime = SetServoLimits(ONTime);    //Make sure the RC Servo Motor values are within its limits
		TmrVal = ONTime;        //Set TmrVal = ONTime
		TmrState = 1;           //Set signal state to be ON for next interrupt
	}
	else if (TmrState == 1)		//If signal is high (ON)
	{
		LATBbits.LATB6 = 0;     //Set low signal for RB6
		TmrVal = OFFTime;       //Set TmrVal = OFFTime
		T1CONbits.TCKPS = 2;	//Change prescaler to 1:64
		TmrState = 0;           //Set signal state to be OFF for next interrupt
	}	
	WriteTimer1(TmrVal);		//Setup Timer1 with the appropriate value to set the interrupt time
	IFS0bits.T1IF = 0;          //Reset Timer1 interrupt flag
	EnableIntT1;                //Enable Timer1 interrupt
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