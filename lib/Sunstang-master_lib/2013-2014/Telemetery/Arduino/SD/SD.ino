#include <SD.h>

#include <DriverInterfaceFrame.h>
#include <FrameFactory.h>
#include <FrameInterface.h>
#include <FrameIO.h>
#include <TestFrame.h>

#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>


File myFile;
FrameIO fio;
int led =13;
FrameInterface* test;
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
  setSyncProvider(RTC.get);  //set which clock to use
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  if (!SD.begin(10)) {
     digitalWrite(led, HIGH);
  }


}

void loop()
{
    //Serial.println("hello");
    test = fio.readDataFrame();
    //Serial.println(test->toString());
  //if the file opened okay, write to it:
  digitalWrite(led, HIGH);
  Serial.write("GOOD");
    myFile = SD.open("driverIn.csv", FILE_WRITE);
    if (myFile) {
      myFile.print(now());
      myFile.print(',');
      myFile.println(test->toString());
      myFile.close();
   }
   digitalWrite(led, LOW);
   delay(2000);
   delete test;
}
