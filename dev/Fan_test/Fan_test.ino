// Generates PWM on pin 5 using analogWrite. Measures tach sensor using interrupt 1.
// Last modified Janurary 25

int fanTach_pin = 3;  // Fan tachometer connected to digital pin 3 (int)
int relay_pin = 4;    // Mosfet connected to digital pin 4
int fanPwm_pin = 5;   // LED connected to digital pin 5 (pwm)
int analog_pin = 0;   // potentiometer connected to analog pin 0

unsigned int rpm = 0;
volatile byte half_revolutions = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(fanPwm_pin, OUTPUT);   // sets the pin as output
  pinMode(relay_pin, OUTPUT);
}


void loop()
{
  int val = analogRead(analog_pin);
  
  analogWrite(fanPwm_pin, val / 4 );  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255

  if (val/4 > 122) digitalWrite(relay_pin, HIGH);
  else digitalWrite(relay_pin, LOW);

  rpm = measureRpm();
  Serial.print("duty cycle = ");Serial.println( 100 - (float(val)/1023 * 100) );
  Serial.print("rpm = ");Serial.println(rpm);
}

unsigned int measureRpm(void)
{
  unsigned int result = 0;
  unsigned long timeold = 0;
  half_revolutions = 0;
  
  timeold = millis();  
  attachInterrupt(digitalPinToInterrupt(fanTach_pin),rpm_fun,FALLING);
  while (half_revolutions < 20); // wait for 20 revolutions  
  result = 30*1000/(millis() - timeold)*half_revolutions;
  detachInterrupt(digitalPinToInterrupt(fanTach_pin));
  return result;
}

void rpm_fun(void)
{
  half_revolutions++;
}


