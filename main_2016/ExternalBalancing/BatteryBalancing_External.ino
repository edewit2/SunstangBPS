const int nGroups = 2;
const int nCellsPerGroup = 3;
const int nCells = nGroups * nCellsPerGroup;
boolean gates[nCells];
float maxValue[nGroups];
float cellVoltages[nCells];
const float threshold = 0.081; // 6A * 0.006750 Ohms
const int completeIndicatorPin = 13;
const int sampleSetupTime = 5;
const int sampleHoldTime = 2;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  gates[0] = HIGH;
  gates[1] = HIGH;
  gates[2] = HIGH;
  gates[3] = HIGH;
  gates[4] = HIGH;
  gates[5] = HIGH;
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(completeIndicatorPin, OUTPUT);
  writeAllGateValues();

  Serial.println(A5);
  Serial.println(A5);
  Serial.println(A6);
  Serial.println(A7);
}
float getLogicalCellVoltage(int raw)
{
  Serial.print("Getting logical voltage ");
  Serial.println(raw);
  // assuming 5.0V ref
  float Vref = 5.00;
  return Vref*(raw/1024.000);
}
uint8_t getGroup(uint8_t cell)
{
  return cell/nCellsPerGroup;
}
uint8_t getGatePin(uint8_t cell)
{
  switch (cell)
  {
    case 0:   return 2;   break;
    case 1:   return 3;   break;
    case 2:   return 4;   break;
    case 3:   return 5;   break;
    case 4:   return 6;   break;
    case 5:   return 7;   break;
    default:  break;
  }
}
uint8_t getBatteryPin(uint8_t cell)
{
  switch (cell)
  {
    case 0:   return A2;   break;
    case 1:   return A3;   break;
    case 2:   return A4;   break;
    case 3:   return A5;   break;
    case 4:   return A6;   break;
    case 5:   return A7;   break;
    default:  break;
  }
}

uint16_t measureVoltage(uint8_t batteryPin, uint8_t gatePin)
{
  // Temporarily disable charging
  boolean state = digitalRead(gatePin);
  digitalWrite(gatePin, HIGH);
  delay(sampleSetupTime);

  // Make the measurement
  uint16_t value = debugAnalogRead(batteryPin);
  delay(sampleHoldTime);
  digitalWrite(gatePin, state);
  return value;
}
uint16_t debugAnalogRead(uint8_t pin)
{
  Serial.print("Reading pin ");
  Serial.println(pin);
  // return analogRead(pin);
  switch(pin)
  {
    case A2:
      return 52;
    case A3:
      return 51;
    case A4:
      return 33;
    case A5:
      return 212;
    case A6:
      return 212;
    case A7:
      return 200;
    default:
      return 0;
  }
}

void writeAllGateValues()
{
  // Write all gate values
  for (int cell = 0; cell < nCells; cell++)
  {
    digitalWrite(getGatePin(cell), gates[cell]);
  }
}

void loop() {
  boolean moduleComplete = false;
  
  // Determine the max voltages on each cell, as well as the maximum
  Serial.println("Step 1");
  for (int group = 0; group < nGroups; group++)
  {
    float maxValOfGroup = 0;
    for (int cell = 0; cell < nCellsPerGroup; cell++)
    {
      int cellNumber = nCellsPerGroup*group+cell;
      int rawCellVoltage = measureVoltage(getBatteryPin(cellNumber), getGatePin(cellNumber));
      float logicalCellVoltage = getLogicalCellVoltage(rawCellVoltage);
      cellVoltages[cellNumber] = logicalCellVoltage;
      Serial.print("cell: ");
      Serial.println(logicalCellVoltage);
      maxValOfGroup = max(maxValOfGroup, logicalCellVoltage);
    }
    maxValue[group] = maxValOfGroup;
    Serial.print("Max of group: ");
    Serial.println(maxValue[group]);
  }
  Serial.println("Step 2");



  uint8_t consecutiveCompleteCells;
  // Determine the desired gate voltage
  for (int cell = 0; cell < nCells; cell++)
  {
    if (cell % nCellsPerGroup == 0)
    {
      consecutiveCompleteCells = 0;
    }

    uint8_t group = getGroup(cell);
    
    float variance = maxValue[group] - cellVoltages[cell];
    Serial.print("Variance for cell ");

    Serial.print(cell);
    Serial.print(":");
    Serial.println(variance);

    if (variance > threshold)
    {
      // Charge the cell
      gates[cell] = LOW;
    }
    else
    {
      // No need to charge this given cell
      gates[cell] = HIGH; 
      consecutiveCompleteCells++;
    }

    if (consecutiveCompleteCells >= nCellsPerGroup)
    {
      moduleComplete = true;
    }
  }

  // Indicate if any module is done

  if (moduleComplete)
  {
    digitalWrite(completeIndicatorPin, HIGH);
  }
  else
  {
    digitalWrite(completeIndicatorPin, LOW);
  }

  writeAllGateValues();
  

  Serial.println("Done iteration\n\n");
  delay(100);
}

