#define DEBUG 0

// include the library code:
#include <LiquidCrystal.h>
#include <mcp_can.h>
#include <SPI.h>
#include <TimerOne.h>
#include <CANHelper.h>
#include <KellyHelper.h>
#include <MPPtHelper.h>

enum kellyState{
 A2D1,
 A2D2,
 MONITOR1,
 MONITOR2,
 ACC,
 BRK,
 REV
};

kellyState waitingFor = A2D1;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3, A2, 5, 6, 7, 4 );
MCP_CAN CAN(10);
bool requestMPPTFlag = false;
bool requestMCFlag = false;
bool recalcSOC = false;
bool updateLCDFlag = false;


volatile uint32_t tick = 0;
uint16_t panel1Current = 0;
uint16_t panel2Current = 0;
float speed = 0;
uint8_t kellyCurrent = 0;

void setup() {
    Serial.begin(115200);  
  Timer1.initialize(0.1e6);  //in us
  Timer1.attachInterrupt(timer);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Intializing CAN");
  lcd.setCursor(0, 1);
  lcd.print("<3 from Peter");
  
  // consider lighting pin 13 LED to confirm CAN connection
  while(CAN.begin(CAN_SPEED) != CAN_OK)
  {
    #if DEBUG == 1
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
    #endif
    delay(100);
  }
  
  //Since we only care about a few packets only ask for those.
 //CAN.init_Mask(0, 0, 0x7F0);                         // there are 2 mask in mcp2515, you need to set both of them
  //CAN.init_Mask(1, 0, 0x7F0);
  
  //CAN.init_Filt(0, 0, 0x060);    //BPS
  //CAN.init_Filt(1, 0, 0x160);    //MC
  //CAN.init_Filt(2, 0, 0x770);    //MPPT
  
  lcd.clear();
  
  //Speed Location
  displayNCharAt(0,0,3,'-');
  
  //State of Charge
  displayNCharAt(0,1,3,'-');
  lcd.print("%");
  
  //Panel Current
  displayNCharAt(10,0,6,'-');
  
  //Motor Current
  displayNCharAt(10,1,6,'-');
  
}

void recieveCAN()
{
  uint32_t id;
  uint8_t buff[8];
  uint8_t len;
    CAN.readMsgBufID(&id, &len, buff); 
    
    if(id == MPPT_1_RESPONSE_CAN_ID)
    {
      panel1Current = getCurrentIn(buff);
      #if DEBUG == 1
        Serial.print("MPPT 1 Says hi!");
      #endif
      //TODO: Convert to Amps
      
    }
    else if(id == MPPT_2_RESPONSE_CAN_ID)
    {
      #if DEBUG == 1
        Serial.print("MPPT 2 Says hi!");
      #endif
      panel2Current = getCurrentIn(buff);
      
      //TODO: Convert to Amps
    }else if(id == KELLY_RESPONSE  && waitingFor == A2D1)
    {
        Serial.print("Throttle: ");
        Serial.print((float)kelly::getThrottleA2D(buff)/255 * 5);
        Serial.println("V");

        Serial.print("Brake: ");
        Serial.print((float)kelly::getBrakeA2D(buff)/255 * 5);
        Serial.println("V");
                      #if DEBUG == 1
        Serial.print("Supply voltage: ");
        Serial.print((kelly::getSupplyVoltage(buff)-120)/28 + 4.75);
        Serial.println("V");
        
        Serial.print("Operational voltage: ");
        Serial.print((float)kelly::getOperationVolatage(buff)/1.39);
        Serial.println("V");
        
        Serial.print("B+ voltage: ");
        Serial.print((float)kelly::getBPlusVoltage(buff)/1.39);
        Serial.println("V");        
      #endif
    }else if(id == KELLY_RESPONSE  && waitingFor == A2D2)
    {
      #if DEBUG == 1
        Serial.print("Current A: ");
        Serial.println(kelly::getCurrentA(buff));
        Serial.println("A");
        
        Serial.print("Voltage A: ");
        Serial.print((float)kelly::getVoltageA(buff)/1.39);
        Serial.println("V");
        
        Serial.print("Current B: ");
        Serial.println(kelly::getCurrentB(buff));
        Serial.println("A");
        
        Serial.print("Voltage B: ");
        Serial.print((float)kelly::getVoltageB(buff)/1.39);
        Serial.println("V");

        Serial.print("Current C: ");
        Serial.println(kelly::getCurrentC(buff));
        Serial.println("A");
        
        Serial.print("Voltage C: ");
        Serial.print((float)kelly::getVoltageC(buff)/1.39);
        Serial.println("V");
      #endif
      
      kellyCurrent = max(kelly::getCurrentA(buff), max(kelly::getCurrentB(buff), kelly::getCurrentC(buff)));
    }else if(id == KELLY_RESPONSE  && waitingFor == MONITOR1)
    {
      #if DEBUG == 1
        Serial.print("PWM: ");
        Serial.println(kelly::getPWM(buff));
      
        Serial.print("Motor enabled: ");
        Serial.println(kelly::isMotorEnabled(buff));
      
        Serial.print("Motor temperature: ");
        Serial.print(kelly::getMotorTemp(buff));
        Serial.println("degrees C");
        
        Serial.print("Controller temperature: ");
        Serial.print(kelly::getControllerTemp(buff));
        Serial.println("degrees C");

        Serial.print("Temperature of high side of \"FETMOS\": ");
        Serial.print(kelly::getHighFetmosTemp(buff));
        Serial.println("degrees C");
        
        Serial.print("Temperature of low side of \"FETMOS\": ");
        Serial.print(kelly::getLowFetmosTemp(buff));
        Serial.println("degrees C");
      #endif
    }
    else if(id == KELLY_RESPONSE  && waitingFor == MONITOR2)
    {
      //speed is in RPM,  
      speed = kelly::getSpeed(buff)  * 2.7928759e-5; //(RPM * 2*PI*r )/ 60s, r =  266.7e-3m (10.5")
      #if DEBUG == 1
        Serial.print("Speed: ");
        Serial.println(speed);
      
        Serial.print("Present current level of Kelly: ");
        Serial.print(kelly::getPresentCurrentPercent(buff));
        Serial.println("%");
        
        if(kelly::getErrorCode(buff)){
          Serial.print("Kelly error: ");
          Serial.println(kelly::getErrorCode(buff), BIN);
        }
        
      #endif 
    }
    else if(id == KELLY_RESPONSE  && waitingFor == ACC)
    {
      #if DEBUG == 1
        Serial.print("Accelerator Switch: ");
        Serial.println(buff[0]);
      #endif
    }
    else if(id == KELLY_RESPONSE  && waitingFor == BRK)
    {
      #if DEBUG == 1
        Serial.print("Brake Switch: ");
        Serial.println(buff[0]);
      #endif
    }
    else if(id == KELLY_RESPONSE  && waitingFor == REV)
    {
      #if DEBUG == 1
        Serial.print("Reverse Switch: ");
        Serial.println(buff[0]);
      #endif
    }
    

    //TODO: Read SC from BPS
    
}

void requestMPPTFrame()
{
  #if DEBUG == 1
    Serial.println("Sending Request to MPPT");
  #endif
  CAN.sendMsgBuf(MPPT_1_REQUEST_CAN_ID, 0, 0, 0);
  CAN.sendMsgBuf(MPPT_2_REQUEST_CAN_ID, 0, 0, 0);
}

void requestMCFrame()
{
  KellyRequest toSend;
  
  switch(waitingFor)
  {
    case A2D1:
      waitingFor = A2D2;
      toSend = CCP_A2D_BATCH_READ2_REQUEST;
      break;
    case A2D2:
      waitingFor = MONITOR1;
      toSend = CCP_MONITOR1_REQUEST;
      break;
    case MONITOR1:
      waitingFor = MONITOR2;
      toSend = CCP_MONITOR2_REQUEST;
      break;
    case MONITOR2:
      waitingFor = ACC;
      toSend = COM_SW_ACC_REQUEST;
      break;
    case ACC:
      waitingFor = BRK;
      toSend = COM_SW_BRK_REQUEST;
      break;
    case BRK:
      waitingFor = REV;
      toSend = COM_SW_REV_REQUEST;
      break;
    case REV:
      waitingFor = A2D1;
      toSend = CCP_A2D_BATCH_READ1_REQUEST;
      break;
  }
  #if DEBUG == 1
    Serial.print("Sending Request to MC: ");
    Serial.println(waitingFor);
  #endif
  CAN.sendMsgBuf(KELLY_REQUEST, 0, toSend.len, toSend.data);  
}

void updateScreen()
{
  //TODO:  Display everything on the LCD
  displaySpeed(speed);
  displayPanelCurrent(-1);
  displayMotorCurrent(kellyCurrent);
  displayStateOfCharge(-1);
}

void timer()
{
  if(tick % 10 == 0)
  {
    requestMPPTFlag = true;
  }
  
  //TODO: Figure out how long a request actually takes.
  if(tick % 5 == 0)
  {
    requestMCFlag = true;
  }
  
  if(tick % 10 == 0)
  {
    updateLCDFlag = true;
  }
  
  tick++;
}

void calculateSOC()
{
    //TODO: Calculate State of Charge 
}

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    recieveCAN();
  }
  
  if(requestMPPTFlag)
  {
    requestMPPTFlag = false;
    requestMPPTFrame();
  }
  
  if(requestMCFlag)
  {
    requestMCFlag = false;
    requestMCFrame(); 
  }
  
  if(recalcSOC)
  {
   calculateSOC(); 
  }
  
  if(updateLCDFlag)
  {
    updateLCDFlag = false;
    updateScreen();
  }
  
  //TODO: Display Stale data.  
}

/**  Displays the speed of the car i
 *   s: speed in KM/h
 **/
void displaySpeed(int s)
{
  displayNCharAt(0,0,3,' ');
  lcd.setCursor(0, 0);
  lcd.print(s);  //Display in Km/h  
}

void displayPanelCurrent(int u)
{
  //Panel Supply
  displayNCharAt(10,0,6,' ');
  lcd.setCursor(10, 0);
  lcd.print(u, 2);
}

void displayMotorCurrent(double u)
{
  displayNCharAt(10,1,6,'M');
  lcd.setCursor(10,1);
  lcd.print(u, 2);
}

void displayStateOfCharge(double u)
{
  //TODO: implement
	displayNCharAt(0,1,3, ' ');
	lcd.setCursor(0,1);
	lcd.print(u, 3);
}


void displayNCharAt(int x, int y, int len, char c)
{
  lcd.setCursor(x, y);
  for(int i=0; i<len; i++)
  {
    lcd.print(c); 
  }
}
