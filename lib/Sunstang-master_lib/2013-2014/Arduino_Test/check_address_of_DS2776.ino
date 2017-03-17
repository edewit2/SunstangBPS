
byte rx[5];
byte address_of_DS2776 [8];

void setup(void) {
  Serial.begin(9600);
  delay(5000);
  
  
  
  //intialize the DS2480B bridge
  Serial.flush();
  Serial.write(0xC1);
  delay(2);
  Serial.flush();
  

  byte data[5];
  //I can't recall what each byte does, but on start of of the serial-1-wire bridge,
  //these commands initalize the bridge
  data[0]=0x17;
  data[1]=0x45;  
  data[2]=0x5B;
  data[3]=0x0F;
  data[4]=0x91;
  Serial.write(data,5);  
  
  
     
  delay(10);  
//  
//   // send data only when you receive data:
//      if (Serial.available() > 0) {
//                // read the incoming byte:
//           for (int x=0; x<5; x++)     {
//                rx[x] = Serial.read();
//           }
//                // say what you got:
//           //Serial.print("I received: ");
//           
//           //for (int x=0; x<5; x++)     {
//           //      Serial.print(rx[x],HEX);
//           //}
//        }
//  delay(10);
//  Serial.write(0xC1);//reset
//  delay(10);
//  Serial.write(0xE1); //put in data mode
//  delay(10);
  
}

void discoverOneWireDevices(void) {
  //search command for device ID
//
//  byte searchCommand[4];
//  searchCommand[0]=0xE1;
//  searchCommand[1]=0xF0;
//  searchCommand[2]=0xE3;
//  searchCommand[3]=0xD5;
//  
//  
//  byte readNetIDCommand[2];
//  readNetIDCommand[0]=0xE1;
//  readNetIDCommand[1]=0x33;
  
  //working code
  byte readROM[13];
  readROM[0]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx
  readROM[1]=0xE1; //Tell bridge to enter data mode
  readROM[2]=0x33; //Command to 1-wire device for Read NET ID
  readROM[3]=0xFF; //buffer for data return
  readROM[4]=0xFF; //buffer for data return
  readROM[5]=0xFF; //buffer for data return
  readROM[6]=0xFF; //buffer for data return
  readROM[7]=0xFF; //buffer for data return  
  readROM[8]=0xFF; //buffer for data return
  readROM[9]=0xFF; //buffer for data return
  readROM[10]=0xFF; //buffer for data return
  readROM[11]=0xE3; //Tell bridge to enter command mode (need to be in command mode to send a reset pulse)
  readROM[12]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. end of tx
  
  //This is the reply from the Read ID Command:
  //----------------------
  //CD -- reply from the bridge that he sent a reset pulse
  //33 -- echo from the bridge of your read ID command
  //3D -- first byte of data (ID)
  //F7 -- data bytes
  //ED -- data bytes
  //0  -- data bytes
  //60 -- data bytes
  //7  -- data bytes
  //0  -- data bytes
  //26 -- data bytes
  //CD -- reply from the bridge that he sent a reset pulse



  for (int i=0; i<13; i++){
    Serial.write(readROM[i], HEX);  
    delay(100);
  }
  
  //readV_IN1();
    
  
  
  delay(8000) ;
  
  //read incoming device ID
//  if (Serial.available() > 0) {
//                // read the incoming bytes:
//           for (int x=0; x< Serial.available(); x++)     {
//                  address_of_DS2776[x] = Serial.read();
//           }
//  }
//  delay(100) ;
  return;
}

void loop(void) {
  
   //for (int x=0; x<5; x++)     {
     //resetOneWire();   //returns CD
     discoverOneWireDevices();
   //}

}

//void resetOneWire(){
//   //send 1-wire reset command before searching for addresses
//   byte resetCommand[2];
//   resetCommand[0]=0xE3;
//   resetCommand[1]=0xC5;
//   Serial.write(resetCommand,2);     
//   delay (2000);
//}

void readV_IN1(){
  byte sendBytes[10];
  sendBytes[0]=0xC1;
  sendBytes[1]=0xE1;
  sendBytes[2]=0xCC; //skip rom command
  sendBytes[3]=0x69;  //read memory command
  sendBytes[4]=0x0C;  //ADDRESS MSB 
  sendBytes[5]=0x0D;
  sendBytes[6]=0xFF;  
  sendBytes[7]=0xE3;
  sendBytes[8]=0xC1;
  //sendBytes[9]=0xC1;
  
  for (int i=0; i<9; i++){
    Serial.write(sendBytes[i]);  
    delay(10);
  }
}

void read_temp(){
  //this function actually sort of works
  byte sendBytes[10];
  sendBytes[0]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx 
  sendBytes[1]=0xE1; //Tell Bridge enter data mode 
  sendBytes[2]=0xCC; //To the chips skip rom command / when we have all the devices on a bus, we will have to use a different command to address the specific device we want to hear from
  sendBytes[3]=0x69; //read memory address command - to the chip
  sendBytes[4]=0x0A;  //ADDRESS MSB, for some reason with this command if I give the chip the MSB, it auto increments to get the LSB
  sendBytes[5]=0xFF; //buffer for 1 byte data return
  sendBytes[6]=0xFF; //buffer for 1 byte data return
  sendBytes[8]=0xE3; //Tell bridge to go back into commmand mode
  sendBytes[9]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. end of tx 
  
  for (int i=0; i<10; i++){
    Serial.write(sendBytes[i]);  
    delay(100); //I have tried a lower delay before (10ms) and it was fine
  }
}
