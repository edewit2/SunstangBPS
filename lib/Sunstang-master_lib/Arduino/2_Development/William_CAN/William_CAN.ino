void setup()
{

  /* add setup code here */
	Serial.begin(9600);
}

void loop()
{
	double x;
	int y;
	//char Character;
	//Character = Serial.read();
	//x = 1;

	x = 0;
	do{
		x = Serial.parseInt();
	} while (!x);


	y = 5*x*x*x + 2*x*x - 4*x + 6;
	//Serial.write(y);
	//Serial.println();
	Serial.print("x=");
	Serial.println(x);
	Serial.println(y);


}
