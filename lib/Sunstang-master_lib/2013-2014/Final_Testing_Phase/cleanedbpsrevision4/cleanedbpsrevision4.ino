

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

    write_setup1(0x60,0x0C);  //*Control Register (00001100-->0x0C), undervoltage condition: 2.60V
    write_setup1(0x64,0xD7);  //*Charge Voltage Register (4.19V)
    write_setup1(0x65,0x01);  //Current-to-detect fully charged state
    write_setup1(0x66,0x96);  //Active-Empty Voltage (2.925 V)
    write_setup1(0x67,0x14);  //Active-Empty Current Register (IAE) guess ~ (0.8A-2.6A) 
    write_setup1(0x69,0xFF);  //RSNSP (highest I can set is 3.9mOhms ... ours is actually 1.5mOhms)
    write_setup1(0x78,0x20);  //OVERCURRENT PROTECTION   (value not perfectly set, controller thinks shut off at 12.2A, might actually be 33A at max Vsns)        
    write_setup1(0x7F,0x5A);  //*Overvoltage Threshold Register (4.189V)
    write_setup1(0x7F,0x5A);
    
      pinMode(13, OUTPUT);

  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
}

void discoverOneWireDevices(void) {
  
    delay(1000);

    read_message_with_ROMID_to_make_JohnHappy();
      
    delay(1000);
  
 
  return;
}

void loop(void) {
  
     discoverOneWireDevices();

}

void read_message_with_ROMID_to_make_JohnHappy(){
  //this function actually sort of works
  byte sendBytes[37];
  sendBytes[0]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx 
  sendBytes[1]=0xE1; //Tell Bridge enter data mode 
  sendBytes[2]=0x55; //To the chips match rom command /
  sendBytes[3]=0x3D;
  sendBytes[4]=0x5B;
  sendBytes[5]=0xF3;
  sendBytes[6]=0x00;
  sendBytes[7]=0x60;
  sendBytes[8]=0x07;
  sendBytes[9]=0x00;
  sendBytes[10]=0xD9;
  sendBytes[11]=0x69; //read memory address command - to the chip
  sendBytes[12]=0x08;  //ADDRESS MSB, for some reason with this command if I give the chip the MSB, it auto increments to get the LSB
  sendBytes[13]=0xFF; //buffer for 1 byte data return
  sendBytes[14]=0xFF; //buffer for 1 byte data return
  sendBytes[15]=0xFF; //buffer for 1 byte data return
  sendBytes[16]=0xFF; //buffer for 1 byte data return
  sendBytes[17]=0xFF; //buffer for 1 byte data return
  sendBytes[18]=0xFF; //buffer for 1 byte data return
  sendBytes[19]=0xFF; //buffer for 1 byte data return
  sendBytes[20]=0xFF; //buffer for 1 byte data return
  sendBytes[21]=0xFF; //buffer for 1 byte data return
  sendBytes[22]=0xFF; //buffer for 1 byte data return
  sendBytes[23]=0xFF; //buffer for 1 byte data return
  sendBytes[24]=0xFF; //buffer for 1 byte data return
  sendBytes[25]=0xFF; //buffer for 1 byte data return
  sendBytes[26]=0xFF; //buffer for 1 byte data return
  sendBytes[27]=0xFF; //buffer for 1 byte data return
  sendBytes[28]=0xFF; //buffer for 1 byte data return
  sendBytes[29]=0xFF; //buffer for 1 byte data return
  sendBytes[30]=0xFF; //buffer for 1 byte data return
  sendBytes[31]=0xFF; //buffer for 1 byte data return
  sendBytes[32]=0xFF; //buffer for 1 byte data return
  sendBytes[33]=0xFF; //buffer for 1 byte data return
  sendBytes[34]=0xFF; //buffer for 1 byte data return
  sendBytes[35]=0xE3; //Tell bridge to go back into commmand mode
  sendBytes[36]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. end of tx 
  
  
  for (int i=0; i<37; i++){
    Serial.write(sendBytes[i]);  
    delay(100); //I have tried a lower delay before (10ms) and it was fine
  }
}

void write_setup1(byte address, byte data){
  //this function actually sort of works
  byte sendBytes[16];
  sendBytes[0]=0x29; //Set Vpp for 512us
  sendBytes[1]=0xC1; //Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx 
  sendBytes[2]=0xE1; //Tell Bridge enter data mode 
  sendBytes[3]=0xCC; //To the chips skip rom command / when we have all the devices on a bus, we will have to use a different command to address the specific device we want to hear from
  sendBytes[4]=0x6C; //write to memory address
  
//ADDRESS MSB, for some reason with this command if I give the chip the MSB, it auto increments to get the LSB
  sendBytes[5]=address;  

//Data byte
  sendBytes[6]=data; 

  sendBytes[7]=0xFF; //buffer for 1 byte data return
  sendBytes[8]=0xFF; 
  sendBytes[9]=0xE3; //Tell bridge to go back into commmand mode
  sendBytes[10]=0xFD; //Generate Program Pluse
  sendBytes[11]=0xE1; //Set Data mode
  sendBytes[12]=0xFF; //Read sent byte
  sendBytes[13]=0xE3; //Tell bridge to go back into commmand mode
  sendBytes[14]=0xC1; //Reset pulse
  
  for (int i=0; i<16; i++){
    Serial.write(sendBytes[i]);  
    delay(100); //I have tried a lower delay before (10ms) and it was fine
  }
}

