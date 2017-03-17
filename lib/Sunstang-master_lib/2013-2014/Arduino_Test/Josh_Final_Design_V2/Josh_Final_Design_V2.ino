#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);


boolean clipping = 0; //clipping indicator variables
byte newData = 0;     //data storage variables
byte prevData = 0;
//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
int frequency = -22;

const short int arraysize(40);
short int sensorValue[arraysize]; //this is the microphone in input 1, the white signal line--recognized as "right"
short int sensorValue2[arraysize]; //this is the microphone in input 2, the blue signal line--recognized as "left"
long int noiseONE(0), noiseTWO(0);

#define FALSE_POSITIVE -1
#define HORN 2
#define SIREN 1
#define SCREAM 0

boolean printedSafe = false;
uint32_t last = 0;

void initSingleADC() {
	cli();

	ADMUX = 0;
	ADMUX |= (1 << REFS0); //set reference voltage

	ADCSRA = 0;
	ADCSRB = 0;

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

	sei();
}

void initTimerADC() {

	cli();//disable interrupts

	ADCSRA = 0;
	ADCSRB = 0;

	ADMUX = 0;
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

uint16_t readADC(uint8_t pin) {
	ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);//select ADC channel with safety mask
	ADCSRA |= (1 << ADSC);//single conversion mode
	while (ADCSRA & (1 << ADSC));// wait until ADC conversion is complete
	return ADC;
}

ISR(ADC_vect) {//when new ADC value ready
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

void setup() {
	Serial.begin(9600);
	Serial.println("Begin Setup");

	lcd.clear();
	lcd.begin(16, 2);
	lcd.print("Hello World");

	micOrientationSetup();
	
	last = millis();
	Serial.println("End Setup");
}

void loop() {
	micOrientation();
	//printSingleReading();
	//printFrequency();
}

inline int8_t identifyNoise() {

	initTimerADC();
	delay(500);

	frequency = 38462 / period;
	
	/*
	500Hz  - Horn
	900Hz  - Siren
	1500Hz - Scream
	*/
	static const int resolution = 250;
	static const int FREQ_HORN = 2000, FREQ_SIREN = 3000, FREQ_SCREAM = 4000;

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

inline void micOrientationSetup() {
	// left/right microphone code
	// set the ambient noise level

	//cli(); // turn off interrupts (and therefore the frequency ADC stuff
	initSingleADC();

	int noise1[60], noise2[60];
	for (int i = 0; i < 60; i++) {

		noise1[i] = readADC(1) - 512;
		noise2[i] = readADC(2) - 512;

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
	noiseONE = noiseONE / 60.0 + 60;//adjust sensitivity of microphones by this addition number
	noiseTWO = noiseTWO / 60.0 + 60;//same for this one
	
	//sei();

	//delay(400);
	//Serial.println("noise one");
	//Serial.println(noiseONE);
	//Serial.println("noise two");
	//Serial.println(noiseTWO);
}

inline void micOrientation() {
	if (!printedSafe) {
		lcd.clear();
		lcd.setCursor(6, 0);
		lcd.print("SAFE");
		printedSafe = true;
	}

	initSingleADC();
	for (short int i = 0; i < arraysize; i++)//take in readings from each microphone and store values to a 40 piece array
	{
		sensorValue[i] = readADC(1) - 512;  // read the input on analog pin 1
		sensorValue2[i] = readADC(2) - 512;  // read the input on analog pin 2

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
		lcd.clear();
		lcd.setCursor(4, 0);
		lcd.print("WARNING!");

		int8_t warning = identifyNoise();

		Serial.print("Warning: ");
		Serial.println(warning);

		if (sensor1avg>sensor2avg) { //Serial.println("white wire");
			switch (warning) {
				case FALSE_POSITIVE:
					lcd.setCursor(0, 1);
					lcd.print("False Alarm");
					break;
				case HORN:
					lcd.setCursor(0, 1);
					lcd.print("FRONT HORN");
					break;
				case SIREN:
					lcd.setCursor(0, 1);
					lcd.print("FRONT SIREN");
					break;
				case SCREAM:
					lcd.setCursor(0, 1);
					lcd.print("FRONT SCREAM");
					break;
			}
			delay(2000);
			printedSafe = false;
		}
		if (sensor1avg < sensor2avg) {
			switch (warning) {
				case FALSE_POSITIVE:
					lcd.setCursor(0, 1);
					lcd.print("False Alarm");
					break;
				case HORN:
					lcd.setCursor(0, 1);
					lcd.print("REAR HORN");
					break;
				case SIREN:
					lcd.setCursor(0, 1);
					lcd.print("REAR SIREN");
					break;
				case SCREAM:
					lcd.setCursor(0, 1);
					lcd.print("REAR SCREAM");
					break;
			}
			delay(2000);
			printedSafe = false;
		}

	}
}

// debug code
void printFrequency() {
	frequency = 38462 / period;
	Serial.print("F = ");
	Serial.println(frequency);
	delay(100);
}

void printSingleReading() {
	Serial.println(readADC(3));
	delay(20);
}