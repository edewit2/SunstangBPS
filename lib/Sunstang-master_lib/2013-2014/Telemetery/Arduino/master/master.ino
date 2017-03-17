#include <Frame.h>
#include <FrameFactory.h>
#include <FrameInterface.h>
#include <FrameIO.h>
#include <TestFrame.h>

#include <SD.h>


File myFile;
FrameIO fio;
int led =13;
TestFrame* test;
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
   //if (!SD.begin(10)) {
  //   digitalWrite(led, HIGH);
  //   return;
  //}

   test = new TestFrame(10, 'S');
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //byte* a = test.toByteArray();

  
  //for(int i = 0; i< test->getLength(); i++)
  //  Serial.println(i);
  
  //f->toString();
  
  // if the file opened okay, write to it:
 // if (myFile) {
 //   myFile.println("Please...");
  //  myFile.close();
  //} else {
  //  digitalWrite(led, HIGH);
  //}
}

void loop()
{
  fio.writeDataFrame(test->toByteArray(), test->getId(), test->getLength());
  delay(2000);

}



