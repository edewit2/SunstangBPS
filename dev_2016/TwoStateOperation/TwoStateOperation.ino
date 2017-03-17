// Define a datatype called CAR_STATE
enum CAR_STATE
{
  STATE_IDLE,   // In this state, the program does nothing except wait for the start button to be pressed. 
  STATE_ACTIVE  // In this state, the program continuously checks battery and jumps to the IDLE state if dangrous levels are detected
};

// This variable declares this mode's state of operation. The default car state is IDLE. 
volatile CAR_STATE carState = STATE_IDLE;

void setup() {
  Serial.begin(9600);
  
  // Set interrupt 0 (found on D2) to trigger the startCarFunction
  attachInterrupt(0, startCarFunction, FALLING);
}

void loop() {
  if (carState == STATE_ACTIVE)
  {
    Serial.println("Active");
    
    // Perform battery check, and set carState=STATE_IDLE if necessary 
  }
  else
  {
    Serial.println("Idle");
  }
  delay(1000);
}

// This function is called anytime the start button is pressed. 
// Important: Pressing this button is THE ONLY WAY to get to the ACTIVE state 
// Any variables modified in this function need to be declared as volatile
void startCarFunction()
{
  // Start button was pressed
  // This function is the only way to enter status STATE_ACTIVE
  carState = STATE_ACTIVE;
}

