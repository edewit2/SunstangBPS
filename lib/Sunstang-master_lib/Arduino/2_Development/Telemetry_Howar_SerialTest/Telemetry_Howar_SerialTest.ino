
int64_t x, y;

void setup()
{
	Serial.begin(9600);
  /* add setup code here */

}

void loop()
{
	// if i write: Serial.print(x,y);  then what will be the output and what does it mean?
	/*
	The above will attempt to write out 'x' with the format 'y'.  typically the format argument is
	specified with the following: {DEC, HEX, BIN}, which is the base number system 'x' should be converted to
	For example:
	Serial.print(5, BIN); //prints 0101 (or simply 101) (binary number)
	Serial.print(254, HEX); //prints FE (hexadecimal number)
	Serial.print(10, DEC); //prints 10  (decimal number -- the one you use everyday)
	However, these are just mappings to integer constants, so when you pass in 'y' as the argument, the compiler
	will process it as such.
	*/
	//Thank you for answering me : )
	//the compiler will process it, but i don't know how on earth it compiles.. but anyway it doesn't really matter

	if (Serial.available() > 0) {
		// since x is of type int64_t, it can only ever hold values for an integer type.
		// when you call parseFloat(), the number returned is a float, but then it is implicitely casted
		// to an int.  This means any decimal digits are lost, and the number is rounded down to the nearest
		// whole number (i.e. int)
		x = Serial.parseFloat();
		y = 5 * pow(x, 3) + 2 * x*x - 4 * x + 6;
		/*
		Serial.read() will read in 1 char from the serial monitor.
		Try using Serial.parseInt() or Serial.parseFloat() instead.
		To convert a char to an integer, you can use this trick:
		char c = '2' // the 'character' 2, not the integer number
		int x = c - '0'; // subtract char '0' -> NOTE:  you must have the quotes, to specify as char, and not int
		*/
		
		//all the float data type in adruino have only two decimal right?
		Serial.print("y = ");
		/*
		To print an int64_t value, you must do a little hack.
		You can find more information in the second post here: http://forum.arduino.cc/index.php?topic=134116.0
		There is no native support for printing this value, so you can convert to characters and print those instead.
		you can store youre value in a float/double instead, and print that though.
		*/
		
		/*
		To answer your question from before, you can print a floating point number as a whole number (int),
		simply by casting it.
		Example:
		float f = 3.14159
		Serial.println( (int) f );
		*/
		
		Serial.println(y);
		Serial.print("x= ");
		Serial.println(x);
		// i use int64_t as the data type, but it seems not supported by println() and print().. do i need to change the print.cpp 
		//and print.h file?
		
	}
}
