// LT_PEC_Calculator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <bitset>

using namespace std;

short calculatePEC(short);
static void print16Bits(short);
static void bitWrite(short&, char, char);
static short bitRead(short, char);
static void printIN(short);
static void printPEC(short);

int _tmain(int argc, _TCHAR* argv[])
{
  // This sample of calculating the PEC for 0x0001 is illustrated in Table 24, page 43 of 6804 Datasheet
  // complete with intermediate values
  print16Bits(calculatePEC(0x0001));
  
  return 0;
}

// This algorithm is described on page 42 of the 6804 Datasheet

short calculatePEC(short DIN)
{
  // 1.Initialize the PEC to 0x0010
  short PEC = 0x0010;
  cout << "PEC: ";
  printPEC(PEC);
  cout << endl;

  // 2. For each bit DIN coming into the PEC register group
  for (int i = 15; i >= 0; i--)
  {
    // Set IN
    short IN = 0x0000;
    bitWrite(IN, 0, bitRead(DIN, i) ^ bitRead(PEC, 14));
    bitWrite(IN, 3, bitRead(IN, 0) ^ bitRead(PEC, 2));
    bitWrite(IN, 4, bitRead(IN, 0) ^ bitRead(PEC, 3));
    bitWrite(IN, 7, bitRead(IN, 0) ^ bitRead(PEC, 6));
    bitWrite(IN, 8, bitRead(IN, 0) ^ bitRead(PEC, 7));
    bitWrite(IN, 10, bitRead(IN, 0) ^ bitRead(PEC, 9));
    bitWrite(IN, 14, bitRead(IN, 0) ^ bitRead(PEC, 13));

    cout << "IN: ";
    printIN(IN);

    // Update 15-bit PEC
    PEC = PEC << 1;
    bitWrite(PEC, 14, bitRead(IN, 14));
    bitWrite(PEC, 10, bitRead(IN, 10));
    bitWrite(PEC, 8, bitRead(IN, 8));
    bitWrite(PEC, 7, bitRead(IN, 7));
    bitWrite(PEC, 4, bitRead(IN, 4));
    bitWrite(PEC, 3, bitRead(IN, 3));
    bitWrite(PEC, 0, bitRead(IN, 0));

    cout << "PEC: ";
    printPEC(PEC);
    cout << endl;
  }

  // 4. Append at the LSB then return
  return PEC << 1;
}

static void bitWrite(short &x, char n, char b)
{
  if (b == 0)
  {
    // Clear: AND
    x = x & ~(0x0001 << n);
  } 
  else
  {
    // Set: OR
    x = x | (0x0001 << n);
  }
}

static short bitRead(short x, char n)
{
  return 0x0001 & (x >> n);
}

static void print16Bits(short data)
{
  bitset<16> bits(data);

  /*for (int i = 0; i < 8; i++)
  {
    bool temp = bits[0];
    bits[i] = bits[15 - i];
    bits[15 - i] = temp;
  }*/
  cout << bits << endl;
}


static void printIN(short IN)
{
  cout << bitRead(IN, 14);
  cout << bitRead(IN, 10);
  cout << bitRead(IN, 8);
  cout << bitRead(IN, 7);
  cout << bitRead(IN, 4);
  cout << bitRead(IN, 3);
  cout << bitRead(IN, 0);
  cout << endl;
}

static void printPEC(short PEC)
{
  for (int i = 14; i >= 0; i--)
  {
    cout << bitRead(PEC, i);
    if (i % 4 == 0)
    {
      cout << " ";
    }
  }
  cout << endl;
}

