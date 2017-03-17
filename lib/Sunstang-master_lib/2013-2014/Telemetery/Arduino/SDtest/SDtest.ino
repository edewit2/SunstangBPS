#include <Frame.h>
#include <FrameFactory.h>
#include <FrameInterface.h>
#include <FrameIO.h>
#include <TestFrame.h>

#include <SD.h>


File myFile;
FrameIO fio;
int led =13;
FrameInterface* test;
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  if (!SD.begin(10)) {
     digitalWrite(led, HIGH);
     return;
  }

  //for(int i = 0; i< test->getLength(); i++)
  //  Serial.println(i);

}

void loop()
{
    //Serial.println("hello");
    test = fio.readDataFrame();
    //Serial.println(test->toString());
  //if the file opened okay, write to it:
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("New Data:");
      myFile.println(test->toString());
      myFile.close();
   } else {
     digitalWrite(led, HIGH);
   }
    delete test;
}



