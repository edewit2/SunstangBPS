#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
//clipping indicator variables
boolean clipping = 0;
//data storage variables
byte newData = 0;
byte prevData = 0;
//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period = 0;
int frequency = -22;
const short int arraysize(40);
short int sensorValue[arraysize]; //this is the microphone in input 1, the white signal line--recognized as "right"
short int sensorValue2[arraysize]; //this is the microphone in input 2, the blue signal line--recognized as "left"
long int noiseONE(0), noiseTWO(0);
#define FALSE_POSITIVE -1
#define HORN 2
#define SIREN 1
#define SCREAM 0
void setup() {
	lcd.clear();
	lcd.begin(16, 2);
	Serial.begin(9600);
	pinMode(13, OUTPUT);//led indicator pin
	enableTimerBasedADC();
	Serial.println("Setup freq done");
	micOrientationSetup();
	Serial.println("Setup complete");
}
inline void enableTimerBasedADC() {
	cli();//disable interrupts
	//set up continuous sampling of analog pin 0
	//clear ADCSRA and ADCSRB registers
	ADCSRA = 0;
	ADCSRB = 0;
	ADMUX |= (1 << REFS0); //set reference voltage
	ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
	ADMUX |= (1 << MUX0); // turn on bit 0 in ADMUX to enable ADC on analog pin 1
	ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
	ADCSRA |= (1 << ADATE); //enabble auto trigger
	ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
	ADCSRA |= (1 << ADEN); //enable ADC
	ADCSRA |= (1 << ADSC); //start ADC measurements
	sei();//enable interrupts
}
uint16_t ReadADC(uint8_t ADCchannel) {
	//select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//single conversion mode
	ADCSRA |= (1 << ADSC);
	// wait until ADC conversion is complete
	while (ADCSRA & (1 << ADSC));
	return ADC;
}
void InitADC() {
	cli();//disable interupts
	ADMUX &= ~(1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
	ADCSRA = 0;
	ADCSRB = 0;
	ADCSRA &= ~(1 << ADATE);
	ADCSRA &= ~(1 << ADIE);
	// Select Vref=AVcc
	ADMUX |= (1 << REFS0);
	//set prescaller to 128 and enable ADC 
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
	sei();//enable interrupts
}
//ISR(ADC_vect) {//when new ADC value ready
//
//	// frequency code
//	prevData = newData;//store previous value
//	newData = ADCH;//get value from A0
//	if (prevData < 127 && newData >= 127) {//if increasing and crossing midpoint
//		period = timer;//get period
//		timer = 0;//reset timer
//	}
ISR(ADC_vect) {
	prevData = newData;//store previous value
	newData = ADCH;//get value from A0
	if (prevData < 127 && newData >= 127) {//if increasing and crossing midpoint
		period = timer;//get period
		timer = 0;//reset timer
	}
	if (newData == 0 || newData == 1023) {//if clipping
		PORTB |= B00100000;//set pin 13 high- turn on clipping indicator led
		clipping = 1;//currently clipping
	}

	timer++;//increment timer at rate of 38.5kHz
}
void loop() {
	//frequencyPrinting();
	if (clipping) {//if currently clipping
		PORTB &= B11011111;//turn off clippng indicator led
		clipping = 0;
	}
	//  frequency = 38462/period; //timer rate/period
	// delay(100);
	micOrientation();
}

inline int8_t identifyNoise() {
	//sei();//enable interrupts
	enableTimerBasedADC();//initialize interrupt ADC

	//for(int i=0;i<10;i++)
	//{
	//	delay(20);
	//}
	unsigned long Beginning = 0;
	Beginning = millis();

	while (millis() - Beginning <= 1500) {}
	
	InitADC();//re initialize regular ADC  also disables interrupts
	frequency = 38462 / period;
	static const int resolution = 50;
	/*
	500Hz  - Horn
	900Hz  - Siren
	1500Hz - Scream
	*/
	static const int FREQ_HORN = 500, FREQ_SIREN = 900, FREQ_SCREAM = 1500;
	Serial.print("F = ");
	Serial.println(frequency);
	if (frequency >= FREQ_HORN - resolution && frequency <= FREQ_HORN + resolution) {
		Serial.println("HORN Warning");
		return 2;
	} else if (frequency >= FREQ_SIREN - resolution && frequency <= FREQ_SIREN + resolution) {
		Serial.println("SIREN Warning");
		return 1;
	} else if (frequency >= FREQ_SCREAM - resolution && frequency <= FREQ_SCREAM + resolution) {
		Serial.println("SCREAM Warning");
		return 0;
	} else {

		return -1;
	}


}
//inline void frequencyPrinting(uint8_t flag) {
//		delay(100);
//		if (clipping) {//if currently clipping
//			PORTB &= B11011111;//turn off clippng indicator led
//			clipping = 0;
//		}
//		frequency = 38462 / period;//timer rate/period
//		//print result
//		Serial.print(frequency);
//		Serial.println(" hz");
//		
//	}
//}
inline void micOrientationSetup() {
	// left/right microphone code
	// set the ambient noise level
	//cli(); // turn off interrupts (and therefore the frequency ADC stuff
	InitADC();
	int noise1[60], noise2[60];
	for (int i = 0; i < 60; i++) {

		noise1[i] = ReadADC(1) - 512;
		noise2[i] = ReadADC(2) - 512;
		Serial.println(noise1[i]);
		Serial.println(noise2[i]);
		if (noise1[i] < 0)
			noise1[i] = noise1[i] * (-1);
		if (noise2[i] < 0)
			noise2[i] = noise2[i] * (-1);
	}
	for (int i = 0; i < 60; i++) {
		noiseONE += noise1[i];
		noiseTWO += noise2[i];
	}
	noiseONE = noiseONE / 60.0 + 20;//adjust sensitivity of microphones by this addition number
	noiseTWO = noiseTWO / 60.0 + 20;//same for this one
	//sei();
	//delay(400);
	//Serial.println("noise one");
	//Serial.println(noiseONE);
	//Serial.println("noise two");
	//Serial.println(noiseTWO);
}
inline void micOrientation() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("SAFE");
	//Serial.println("looping");
	for (short int i = 0; i < arraysize; i++)//take in readings from each microphone and store values to a 40 piece array
	{
		sensorValue[i] = ReadADC(1) - 512;  // read the input on analog pin 1
		sensorValue2[i] = ReadADC(2) - 512;  // read the input on analog pin 2
		if (sensorValue[i] < 0)
			sensorValue[i] = sensorValue[i] * (-1);
		if (sensorValue2[i] < 0)
			sensorValue2[i] = sensorValue2[i] * (-1);
	}
	long int sensor1avg(0), sensor2avg(0);
	long int sensor1total(0), sensor2total(0);
	for (short int i = 0; i < arraysize; i++)//add the numbers
	{
		sensor1total += sensorValue[i];
		sensor2total += sensorValue2[i];
	}
	sensor1avg = sensor1total / 40.0;    //average the numbers 
	sensor2avg = sensor2total / 40.0;
	//Serial.println(sensor1avg);

	/*
	THIS SECTION IS TO MONITOR ON THE SERIAL MONITOR WHAT IS GOING ON
	Serial.println("reading ONE");
	Serial.println(analogRead(A1));
	Serial.println("reading TWO");
	Serial.println(analogRead(A2));
	Serial.println('1');
	Serial.println(sensor1avg);
	Serial.println('2');
	Serial.println(sensor2avg);
	*/
	// print out the value you read:
	if (noiseONE < sensor1avg || noiseTWO<sensor2avg) {
		Serial.println("Before identifyNoise()");
		int8_t warning = identifyNoise();
		Serial.print("Warning: ");
		Serial.println(warning);
		if (sensor1avg>sensor2avg) { //Serial.println("white wire");
			lcd.setCursor(0, 0);
			lcd.print("FRONT WARNING!");
			switch (warning) {
				case FALSE_POSITIVE:
					lcd.setCursor(1, 0);
					lcd.print("Oh hi there");
					break;
				case HORN:
					lcd.setCursor(1, 0);
					lcd.print("HORN");
					break;
				case SIREN:
					break;
				case SCREAM:
					break;
			}
			delay(500);
		}
		if (sensor1avg < sensor2avg) {
			// Serial.println("blue wire");
			lcd.setCursor(0, 0);
			lcd.print("REAR WARNING !");
			switch (warning) {
				case FALSE_POSITIVE:
					lcd.setCursor(1, 0);
					lcd.print("Oh hi there");
					break;
				case HORN:
					lcd.setCursor(1, 0);
					lcd.print("HORN");
					break;
				case SIREN:
					break;
				case SCREAM:
					break;
			}
			delay(500);
		}
	}
}