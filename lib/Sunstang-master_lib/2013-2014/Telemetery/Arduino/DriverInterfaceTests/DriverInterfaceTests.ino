#include <DriverInterfaceFrame.h>
#include <FrameFactory.h>
#include <FrameInterface.h>
#include <FrameIO.h>
#include <TestFrame.h>

FrameIO fio;
int led =13;
DriverInterfaceFrame test[] = {DriverInterfaceFrame(0, 0, 0), DriverInterfaceFrame(0xFF, 0xFF, 0xFFFF), DriverInterfaceFrame('S', 'E', 'X')};
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop()
{
  fio.writeDataFrame(test);
  delay(100);
  fio.writeDataFrame(test+1);
  delay(100);
  fio.writeDataFrame(test+2);
  delay(100);
}



