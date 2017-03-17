
struct Click {
	int button;
	int delayTime;
} click[] = {
	{3, 28000},
	{4, 5000},
	{5, 7000}
};

int buttons[] = {3, 4, 5};

void setup() {
	for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
		pinMode(click[i].button, OUTPUT);
	}
	pinMode(13, OUTPUT);
}

void loop() {

	digitalWrite(5, HIGH);
	digitalWrite(13, HIGH);
	delay(500);
	digitalWrite(5, LOW);
	digitalWrite(13, LOW);
	delay(500);

	//for (int i = 0; i < 3; i++) {
	//	digitalWrite(click[i].button, LOW);
	//	digitalWrite(13, HIGH);
	//	delay(1000);
	//	pinMode(click[i].button, INPUT);
	//	digitalWrite(13, LOW);
	//	delay(click[i].delayTime);
	//}
}
