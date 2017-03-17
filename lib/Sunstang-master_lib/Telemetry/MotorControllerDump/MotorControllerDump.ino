
// include the library code:
#include <mcp_can.h>
#include <SPI.h>
#include <TimerOne.h>
#include <CANHelper.h>
#include <KellyHelper.h>

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
MCP_CAN CAN(10);
bool requestMCFlag = false;


volatile uint32_t tick = 0;
void setup() {
  Serial.begin(115200);  
  Timer1.initialize(0.1e6);  //in us
  Timer1.attachInterrupt(timer);
  
  while(CAN.begin(CAN_SPEED) != CAN_OK)
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  } 
}

float brakeA2D;
float throttleA2D;
float supplyA2D;
float operationA2D;
float BPlusA2D;

int8_t currentA;
int8_t currentB;
int8_t currentC;

float voltageA;
float voltageB;
float voltageC;

uint8_t PWM;
bool motorEnabled;
uint8_t motorTemp;
uint8_t controllerTemp;
uint8_t highMosfet;
uint8_t lowMosfet;

int16_t RPM;
int8_t currentPercent;
int16_t error;

bool throttle;
bool brake;
bool reverse;

void recieveCAN()
{
  uint32_t id;
  uint8_t buff[8];
  uint8_t len;
    CAN.readMsgBufID(&id, &len, buff); 
   if(id == KELLY_RESPONSE  && waitingFor == A2D1)
    {
      throttleA2D = (float)kelly::getThrottleA2D(buff)/255 * 5;
      brakeA2D =(float)kelly::getBrakeA2D(buff)/255 * 5;
      supplyA2D = (kelly::getSupplyVoltage(buff)-120)/28 + 4.75;
      operationA2D = (float)kelly::getOperationVolatage(buff)/1.39;
      BPlusA2D  =(float)kelly::getBPlusVoltage(buff)/1.39;
    }else if(id == KELLY_RESPONSE  && waitingFor == A2D2)
    {
        currentA = kelly::getCurrentA(buff);
        voltageA = (float)kelly::getVoltageA(buff)/1.39;
        
        currentB = kelly::getCurrentB(buff);
        voltageB =(float)kelly::getVoltageB(buff)/1.39;

        currentC = kelly::getCurrentC(buff);
        voltageC = (float)kelly::getVoltageC(buff)/1.39;
    }else if(id == KELLY_RESPONSE  && waitingFor == MONITOR1)
    {
       PWM =kelly::getPWM(buff);
     
        motorEnabled = kelly::isMotorEnabled(buff);
      
        motorTemp = kelly::getMotorTemp(buff);
        
       controllerTemp = kelly::getControllerTemp(buff);

        highMosfet = kelly::getHighFetmosTemp(buff);
        
        lowMosfet = kelly::getLowFetmosTemp(buff);
    }
    else if(id == KELLY_RESPONSE  && waitingFor == MONITOR2)
    {
      //speed is in RPM,  
      //speed = kelly::getSpeed(buff)  * 2.7928759e-5; //(RPM * 2*PI*r )/ 60s, r =  266.7e-3m (10.5")
        
        RPM  = kelly::getSpeed(buff);      
        currentPercent =  kelly::getPresentCurrentPercent(buff);
        
          error = kelly::getErrorCode(buff);
    }
    else if(id == KELLY_RESPONSE  && waitingFor == ACC)
    {
        throttle = buff[0];
    }
    else if(id == KELLY_RESPONSE  && waitingFor == BRK)
    {
        
        brake = buff[0];
    }
    else if(id == KELLY_RESPONSE  && waitingFor == REV)
    {
        reverse = buff[0];
    }

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


void timer()
{
  
  //TODO: Figure out how long a request actually takes.
  if(tick % 5 == 0)
  {
    requestMCFlag = true;
  }
    
  tick++;
}

void updateOut(){
   Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  
   Serial.print("Throttle: ");
        Serial.print(throttleA2D);
        Serial.println("V");

        Serial.print("Brake: ");
        Serial.print((float)brakeA2D);
        Serial.println("V");

        Serial.print("Supply voltage: ");
        Serial.print(supplyA2D);
        Serial.println("V");
        
        Serial.print("Operational voltage: ");
        Serial.print(operationA2D);
        Serial.println("V");
        
        Serial.print("B+ voltage: ");
        Serial.print(BPlusA2D);
        Serial.println("V"); 
     
      Serial.print("Current A: ");
        Serial.print(currentA);
        Serial.println("A");
        
        Serial.print("Voltage A: ");
        Serial.print(voltageA);
        Serial.println("V");
        
        Serial.print("Current B: ");
        Serial.print(currentB);
        Serial.println("A");
        
        Serial.print("Voltage B: ");
        Serial.print(voltageB);
        Serial.println("V");

        Serial.print("Current C: ");
        Serial.print(currentC);
        Serial.println("A");
        
        Serial.print("Voltage C: ");
        Serial.print(voltageC);
        Serial.println("V");
  
        Serial.print("PWM: ");
        Serial.println(PWM);
      
        Serial.print("Motor enabled: ");
        Serial.println(motorEnabled);
      
        Serial.print("Motor temperature: ");
        Serial.print(motorTemp);
        Serial.println("degrees C");
        
        Serial.print("Controller temperature: ");
        Serial.print(controllerTemp);
        Serial.println("degrees C");

        Serial.print("Temperature of high side of \"FETMOS\": ");
        Serial.print(highMosfet);
        Serial.println("degrees C");
        
        Serial.print("Temperature of low side of \"FETMOS\": ");
        Serial.print(lowMosfet);
        Serial.println("degrees C");
    
        Serial.print("Speed: ");
        Serial.print(RPM);
        Serial.println("RPM");
      
        Serial.print("Present current level of Kelly: ");
        Serial.print(currentPercent);
        Serial.println("%");
        Serial.print("Kelly error: ");
        Serial.println(error, HEX);
        if(error != 0)
        {
         if(error & 0x8000){
         //44
         Serial.println("\tUknown error: 0x44");
         }
         
         if(error & 0x4000){
           //43
           Serial.println("\tMotor Over Temp: 0x43");
         }
         
         if(error & 0x2000){
           //42
           Serial.println("\tHall sensor error: 0x42");
         } 
         
         if(error & 0x1000){
           //41
           Serial.println("\tRegen or Startup over voltage: 0x41");
         }
         
         if(error & 0x0800){
           //34
           Serial.println("\tNon-zero throttlo on direction change: 0x31");
         }
         
         if(error & 0x0400){
           //33
           Serial.println("\tHall throttle is open or short circuit: 0x33");
         }
         
         if(error & 0x0200){
           //32
           Serial.println("\tInternal reset: 0x32");
         } 
         
         if(error & 0x0100){
           //31
           Serial.println("\tFrequent error: 0x31");
         } 
         
         if(error & 0x0080){
           //24
           Serial.println("\tThrottle Error at power up: 0x24");
         }
       
       if(error & 0x0040){
         //23
         Serial.println("\tController over temperature: 0x23");
       }
       
       if(error & 0x0020){
         //22
         Serial.println("\tInteral voltage fault: 0x22");
       } 
       
       if(error & 0x0010){
         //21
         Serial.println("\tMotor Did not start: 0x21");
       }
       
       if(error & 0x0008){
         //14
         Serial.println("\tController Over Temp warning: 0x14");
       }
       
       if(error & 0x0004){
         //13
         Serial.println("\tLow voltage error(on high power): 0x13");
       }
       
       if(error & 0x0002){
         //12
         Serial.println("\tOver voltage error on high power side: 0x12");
       } 
       
       if(error & 0x0001){
         //11
         Serial.println("\tUknown error: 0x11");
       }     
        }
       
        
        Serial.print("Accelerator Switch: ");
        Serial.println(throttle);

        Serial.print("Brake Switch: ");
        Serial.println(brake);

        Serial.print("Reverse Switch: ");
        Serial.println(reverse);
}

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    recieveCAN();
    updateOut();
  }
  
    
  if(requestMCFlag)
  {
    requestMCFlag = false;
    requestMCFrame(); 
  }
}
