#include <Frame.h>
#include <FrameFactory.h>
#include <FrameInterface.h>
#include <FrameIO.h>
#include <TestFrame.h>

#include <SD.h>


File myFile;
FrameIO fio;
int led =13;
  TestFrame test[] = {TestFrame(80, 'a', 0xFFFF), TestFrame('S', 'E', 'X'), TestFrame('P', 'W', 'A')};
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  //fio.writeDataFrame(test);
  fio.writeDataFrame(test+1);
  //  fio.writeDataFrame(test+2);
}

void loop()
{
  fio.writeDataFrame(test);
  delay(1000);
  fio.writeDataFrame(test+1);
  delay(1000);
  fio.writeDataFrame(test+2);
  delay(1000);
}



