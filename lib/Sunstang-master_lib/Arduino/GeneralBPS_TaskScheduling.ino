#include<TimerOne.h>


const unsigned long	TIMER_PERIOD = 500000; // Period = 500000us or 0.5s
const unsigned long	TASK_PERIOD_CheckBattery = 500000; // 0.5s
const unsigned long	TASK_PERIOD_UpdateLCD = 1000000; // 1s
const unsigned long	TASK_PERIOD_BroadcastMeasurements = 1000000; // 1s

const unsigned long 	TASK_PRESCALAR_CheckBattery = TASK_PERIOD_CheckBattery / TIMER_PERIOD;
const unsigned long 	TASK_PRESCALAR_UpdateLCD = TASK_PERIOD_UpdateLCD / TIMER_PERIOD;
const unsigned long 	TASK_PRESCALAR_BroadcastMeasurements = TASK_PERIOD_BroadcastMeasurements / TIMER_PERIOD;


volatile boolean newTimeSlice = false;
boolean task_CheckBattery_Ready = false;
boolean task_UpdateLCD_Ready = false;
boolean task_BroadcastMeasurements_Ready = false;

// MAIN CODE

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");
  
  Serial.print("Check battery prescalar:"); 
  Serial.println(TASK_PRESCALAR_CheckBattery);
  
  Serial.print("Check battery prescalar:"); 
  Serial.println(TASK_PRESCALAR_UpdateLCD);
  
  Serial.print("Check battery prescalar:"); 
  Serial.println(TASK_PRESCALAR_BroadcastMeasurements);
  
  // This timer is what makes our system real-time. See the comment above timerISR()
  Timer1.initialize(TIMER_PERIOD);  
  Timer1.attachInterrupt(timerISR); 
}


// Use time-slice scheduler
// At the beginning of each time-slice, the scheduler waits for the timer event, then tasks are given an opportunity to run, if ready
void loop() {

  waitForTimeSlice();

  // task_CheckBattery checks the voltages of batteries and controls both relays accordingly
  if (task_CheckBattery_Ready)
  {
    task_CheckBattery();
    task_CheckBattery_Ready = false;
  }

  // task_UpdateLCD writes to the LCD screen the minimum/maximum voltages (and the status of the controller if not running)
  if (task_UpdateLCD_Ready)
  {
    task_UpdateLCD();
    task_UpdateLCD_Ready = false;
  }

  // task_BroadcastMeasurements broadcasts the battery voltages on the canbus
  if (task_BroadcastMeasurements_Ready)
  {
    task_BroadcastMeasurements();
    task_BroadcastMeasurements_Ready = false;
  }
}

// This function is called automatically every 'TIMER_PERIOD' microseconds
// Note the real-time nature of this type of programming; we are not relying on delays or specific function execution times.
// Instead, we rely on a free-running timer (independant of our program) to trigger certain functions at EXACT intervals.
void timerISR()
{
  // Raise timeslice flag
  newTimeSlice = true;
}
void waitForTimeSlice()
{
  // poll timeslice flag. Reset flag upon exit
  while (newTimeSlice == false);
  newTimeSlice = false;

  // At the very beginning of program runtime, initialize count variables used for prescaling
  static unsigned char checkBatteryCount = 0;
  static unsigned char updateLcdCount = 0;
  static unsigned char broadcastMeasurementsCount = 0;

  // Determine if any of the tasks is ready to run using prescalar value
  
  if (checkBatteryCount >= TASK_PRESCALAR_CheckBattery)
  {
    task_CheckBattery_Ready = true;
    checkBatteryCount = 0;
  }
  
  if (updateLcdCount >= TASK_PRESCALAR_UpdateLCD)
  {
    task_UpdateLCD_Ready = true;
    updateLcdCount = 0;
  }
  
  if (broadcastMeasurementsCount >= TASK_PRESCALAR_BroadcastMeasurements)
  {
    task_BroadcastMeasurements_Ready = true;
    broadcastMeasurementsCount = 0;
  }
  
  // For each task, update count used for prescaling 
  checkBatteryCount++;
  updateLcdCount++;
  broadcastMeasurementsCount++;
}

// TASK DEFINITIONS: 
// You can write specific tasks in these functions and they will automatically get called at the task period intervals defined at the top of this file.

void task_CheckBattery()
{
  Serial.println("Task 1: Monitor");
}

void task_UpdateLCD()
{
  Serial.println("Task 2: LCD"); 
}

void task_BroadcastMeasurements()
{
  Serial.println("Task 3: Canbus");
}
