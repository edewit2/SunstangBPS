/*
 NOT WORKING
 */
#include <stdio.h>
#include <stdlib.h>
#include <i2c.h>
#include "SunstangBPS_temp.h"

#define USE_OR_MASKS

void slaveRead(void);
void slaveTransmit(void);
void i2c_idle(void);
signed char i2c_writeString( unsigned char * );
signed char i2c_writeChar( unsigned char );

unsigned char I2C_Send[5] = {9,1,0,5,NULL};
unsigned char I2C_Recv[5];


void main(void) {
    
    unsigned char sync_mode=0,slew=0,w; 
    
    for(w=0;w<1;w++)
        I2C_Recv[w] = 0;
    
    CloseI2C();
    
    // Initialize I2C module
    sync_mode = SLAVE_7;
    slew = SLEW_OFF;
    OpenI2C(sync_mode,slew);
    
    SSPADD = 0x10;      // Slave address
    
    TRISB = 0;
    LATB = 0;
    
    while(1){
        
        slaveRead();
        LATB = I2C_Recv[0];

        //slaveTransmit();
        
    }
    
}

// Slave reads data from master
void slaveRead(void)
{
    unsigned char temp, length = 0;
    // Read the address sent by master from buffer
    while(DataRdyI2C()==0);         // Wait until the data is transmitted
    while ( !SSPSTATbits.BF );      // wait until byte received  
    temp = SSPBUF;                  // return with read byte 
    
    // Data reception from master by slave
    do{
        while(DataRdyI2C()==0);
          while ( !SSPSTATbits.BF );      // wait until byte received  
          I2C_Recv[length++]=SSPBUF;
    }while(length<5);
    
}

// Slave transmits data requested by master
void slaveTransmit(void)
{
    unsigned char temp;
    while(SSPSTATbits.S!=1);    // Wait until stop condition
    
    // Read the address sent by master from buffer
    while(DataRdyI2C()==0);         // Wait until the data is transmitted
    while ( !SSPSTATbits.BF );      // wait until byte received  
    temp = SSPBUF;                  // return with read byte 
    
    // Slave transmission
    if(SSPSTAT & 0x04)   // Check if master is ready for reception
        while( i2c_writeString(I2C_Send) );
        
}


/********************************************************************
*     Function Name:    i2c_writeString                                    *
*     Return Value:     error condition status                      *
*     Parameters:       address of write string storage location    *
*     Description:      This routine writes a string to the I2C bus,*
*                       until a null character is reached. If Master*
*                       function putcI2C is called. When trans-     *
*                       mission is complete then test for ack-      *
*                       nowledge bit. If Slave transmitter wait for *
*                       null character or not ACK received from bus *
*                       device.                                     *
********************************************************************/
signed char i2c_writeString( unsigned char *wrptr )
{
    unsigned char temp;
    unsigned char i = 1;
    while ( i < 5 )                // transmit data until null character 
    {
        if ( SSPM3 )      // if Master transmitter then execute the following
        {
          temp = i2c_writeChar ( *wrptr );
          if ( temp ) return ( temp );            // return with write collision error

    //      if ( putcI2C ( *wrptr ) )   // write 1 byte
    //      {
    //        return ( -3 );            // return with write collision error
    //      }
    //      IdleI2C();                  // test for idle condition
    //      if ( SSPCON2bits.ACKSTAT )  // test received ack bit state
    //      {
    //        return ( -2 );            // bus device responded with  NOT ACK
    //      }                           // terminate putsI2C() function
        }

        else                          // else Slave transmitter
        {
          SSPIF = 0;         // reset SSPIF bit
          SSPBUF = *wrptr;            // load SSPBUF with new data
          CKP = 1;        // release clock line 
          while ( !SSPIF );  // wait until ninth clock pulse received

          if ( ( !R_W ) && ( !BF ) )// if R/W=0 and BF=0, NOT ACK was received
          {
            return ( -2 );            // terminate PutsI2C() function
          }
        }

        wrptr++;                       // increment pointer
        i++;

    }  // continue data writes until null character
    
    return (0);
    
}
     

/********************************************************************
*     Function Name:    i2c_writeChar                                    *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C bus.               *
*     Description:      This routine writes a single byte to the    * 
*                       I2C bus.                                    *
********************************************************************/
signed char i2c_writeChar( unsigned char data_out )
{
    SSPBUF = data_out;           // write single byte to SSPBUF
    if ( WCOL )      // test if write collision occurred
        return ( -1 );              // if WCOL bit is set return negative #
    else
    {
        if( ((SSPCON1&0x0F)!=0x08) && ((SSPCON1&0x0F)!=0x0B) )  //Slave mode only
        {
            CKP = 1;        // release clock line 
            while ( !SSPIF );  // wait until ninth clock pulse received

            if ( ( !R_W ) && ( !BF ) )// if R/W=0 and BF=0, NOT ACK was received
            {
                return ( -2 );           //return NACK
            }
            else
            {
                return ( 0 );               //return ACK
            }   
        }
        else if( ((SSPCON1&0x0F)==0x08) || ((SSPCON1&0x0F)==0x0B) ) //master mode only
        { 
            while( BF );   // wait until write cycle is complete   
            i2c_idle();                 // ensure module is idle
            if ( ACKSTAT ) // test for ACK condition received
                return ( -2 );          // return NACK
            else return ( 0 );              //return ACK
        }

    }
}


/**********************************************************************************************
Function Prototype : void IdleI2C(void) 

Include            : i2c.h 

Description        : This function generates Wait condition until I2C bus is Idle. 

Arguments          : None 

Return Value       : None 

Remarks            : This function will be in a wait state until Start Condition Enable bit,
                     Stop Condition Enable bit, Receive Enable bit, Acknowledge Sequence
                     Enable bit of I2C Control register and Transmit Status bit I2C Status
                     register are clear. The IdleI2C function is required since the hardware
                     I2C peripheral does not allow for spooling of bus sequence. The I2C
                     peripheral must be in Idle state before an I2C operation can be initiated
                     or write collision will be generated.
***********************************************************************************************/
void i2c_idle( void )
{
  while ( ( SSPCON2 & 0x1F ) || ( R_W ) )
     continue;
}