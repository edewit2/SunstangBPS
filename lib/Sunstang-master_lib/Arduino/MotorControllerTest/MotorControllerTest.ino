#define A1OutPin 2
#define A2OutPin 3
#define BOutPin 4
#define COutPin 5

#define AInPin 7
#define BInPin 8
#define CInPin 9

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT); //A1
  pinMode(3, OUTPUT); //A2
  pinMode(4, OUTPUT); //B
  pinMode(5, OUTPUT); //C

  pinMode(AInPin, INPUT); //A
  pinMode(BInPin, INPUT); //B
  pinMode(CInPin, INPUT); //

}

bool A,B,C;
bool A1out, A2out, Bout, Cout;

void loop() {
  //Read Halls  
  A = digitalRead(AInPin);
  B = digitalRead(BInPin);
  C = digitalRead(CInPin);
  
  //Decide what outputs to use
  if(A)
  {
    A1out = LOW;
    A2out = HIGH;
    B = LOW;
    C = LOW;
  }
  else if(B)
  {
    A1out = HIGH;
    A2out = LOW;
    Bout = HIGH;
    Cout = LOW;
  }
  else //c, I'm going to assume that we'll always get a reading.  This will execute if any of the halls are broken.  TODO: Add error handling 
  {
    A1out = HIGH;
    A2out = LOW;
    Bout = LOW;
    Cout = HIGH;
  }
  
  //Set outputs
  digitalWrite(A1OutPin, A1out);
  digitalWrite(A2OutPin, A2out);
  digitalWrite(BOutPin, Bout);
  digitalWrite(COutPin, Cout);
  
}
