// this code is written, for implementing a charge controller based on AVR achitecture.
// has a maximum input voltage of 100VDC
// any parameter could be changed in the code.
// more details are in the code.
// do not change a value you are not told to change at will because it can affect the entire behaviour and output.

const int chargePin = 3; // I/O 3, connects to the gate of the switching pMOSFET.
const int input = 5; // A5, connects to the voltage divider at the input side.
const int battery = 4; // A4, connects to the voltage divider at the output side.
const float chargeVoltage = 12.75; // change this to the voltage of your battery at full charge.
const float minimumChargingVoltage = 13.0; // change this to the lowest voltage required to charge your battery
const float maximumChargingVoltage = 14.0; // change this to the highest voltage required to charge your battery.
const float maximumChargerVoltage = 100.0; // change this to the highest voltage you want to give the charge controller's input.
const float maxBattery = 100.0; 
const float lowVoltage = 10.5; // change this to the voltage of your battery when it is low.
int batteryPercentage;
float chargerVoltage;
float batteryVoltage;
const int chargingIndicator = 9; // connect this pin to an led (prefferably blue) for indicating charging process.
const int lowIndicator = 10; // connect this pin to an led (prefferably red) for indicating battery's critical charge level.
const int MPPTIndicator = 11; // connect this pin to an led (prefferably green) for indicating MPPT mode.
const int p25LED = 8; // connect this pin to an led for indicating 25% charge.
const int p50LED = 12; // connect this pin to an led for indicating 50% charge.
const int p75LED = 7; // connect this pin to an led for indicating 75% charge.
const int p100LED = 6; // connect this pin to an led for indicating 100% charge.
void setup() {
  pinMode(p25LED, OUTPUT);
  pinMode(chargingIndicator, OUTPUT);
  pinMode(lowIndicator, OUTPUT);
  pinMode(MPPTIndicator, OUTPUT);
  pinMode(p50LED, OUTPUT);
  pinMode(p75LED, OUTPUT);
  pinMode(p100LED, OUTPUT);
  TCCR2B = TCCR2B & B11111000 | B00000001;
  pinMode(chargePin, OUTPUT);
  analogWrite(chargePin, 255);
  Serial.begin(9600);

}

void loop() {
  chargerVoltage = measureInputVoltage();
  batteryVoltage = measureBatteryVoltage();
  Serial.println("chargerVoltage = " + String(chargerVoltage));
  Serial.println("batteryVoltage = " + String(batteryVoltage));
  calibratePercentage();
  if(withinRange() && !IsFullyCharged())
  { // check if battery isn't fully charged and...
    delay(0);
    analogWrite(chargePin, 1); // ...charge in normal mode.
    digitalWrite(chargingIndicator, 1);
    digitalWrite(MPPTIndicator, 0);
    Serial.println(255 - 0);
    Serial.println("charging in Normal mode");
  }
  else if(InputIsHigh() && !IsFullyCharged())
  { // check if battery isn't fully charged and...
    float volt = measureInputVoltage();
    int dutyCycle = (255 * maximumChargingVoltage) / volt;
    dutyCycle = constrain(dutyCycle, 0, 255);
    analogWrite(chargePin, 255 - dutyCycle); // ...charge in MPPT mode.
    digitalWrite(chargingIndicator, 1);
    digitalWrite(MPPTIndicator, 1);
    Serial.println(dutyCycle);
     Serial.println("charging in MPPT mode");
  }
  else if(withinRange() && IsFullyCharged())
  { // check if battery is fully charged and...
    delay(0);
    analogWrite(chargePin, 255); // ...stop charging.
    digitalWrite(chargingIndicator, 0);
    digitalWrite(MPPTIndicator, 0);
    Serial.println(255 - 255);
    Serial.println("battery full!");
  }
  else if(InputIsHigh() && IsFullyCharged())
  { // check if battery is fully charged and...
    delay(0);
    analogWrite(chargePin, 255); // ...stop charging.
    digitalWrite(chargingIndicator, 0);
    digitalWrite(MPPTIndicator, 1);
    Serial.println(255 - 255);
    Serial.println("battery full!");
  }
  
}

float measureInputVoltage()
{ // function that measures input voltage.
  //pinMode(chargePin, INPUT_PULLUP);
  analogWrite(chargePin, 255);
  delay(5);
  float ADCval = analogRead(input);
  float voltage = (ADCval * maximumChargerVoltage) / 1023.0;
  //pinMode(chargePin, OUTPUT);
  return voltage;
}
float measureBatteryVoltage()
{ // function that measures battery voltage.
  //pinMode(chargePin, INPUT_PULLUP);
  analogWrite(chargePin, 255);
  delay(5);
  float ADCval = analogRead(battery);
  float voltage = (ADCval * maxBattery) / 1023.0;
  //pinMode(chargePin, OUTPUT);
  return voltage;
}

bool withinRange()
{ // function that checks if input voltage is withing range.
  float volt = measureInputVoltage();
  if (volt >= minimumChargingVoltage && volt <= maximumChargingVoltage)
  {
    return true;
  }
  else {
    return false;
  }
}
bool IsFullyCharged()
{ // function that checks if battery is fully charged.
  if (measureBatteryVoltage() >= chargeVoltage)
  {
    return true;
  }
  else {
    return false;
  }
}
bool InputIsHigh()
{ // function that checks if input voltage is higher than normal.
  float volt = measureInputVoltage();
  if (volt > maximumChargingVoltage)
  {
    return true;
  }
  else {
    return false;
  }
}
 float fmap(float val, float fromLow, float fromHigh, float toLow, float toHigh)
 {
  float norm = (val - fromLow) / (fromHigh - fromLow);
  float lerp = norm * (toHigh - toLow) + toLow;
  return lerp;
 }
 void calibratePercentage() {
  if(batteryVoltage < lowVoltage)
  {
    batteryPercentage = 0;
  }
  else if (batteryVoltage > chargeVoltage)
  {
    batteryPercentage = 100;
  }
  else {
    batteryPercentage = fmap(batteryVoltage, lowVoltage, chargeVoltage, 0, 100.0);
  }
if(batteryPercentage >= 25)
{
  digitalWrite(p25LED, 1);
}
else {
  digitalWrite(p25LED, 0);
}
if(batteryPercentage >= 50)
{
  digitalWrite(p50LED, 1);
}
else {
  digitalWrite(p50LED, 0);
}
if(batteryPercentage >= 75)
{
  digitalWrite(p75LED, 1);
}
else {
  digitalWrite(p75LED, 0);
}
if(batteryPercentage == 100)
{
  digitalWrite(p100LED, 1);
}
else {
  digitalWrite(p100LED, 0);
}
if(batteryPercentage == 0)
{
  digitalWrite(lowIndicator, 1);
}
else {
  digitalWrite(lowIndicator, 0);
}
  Serial.println("battery percentage = " + String(batteryPercentage) + "%");
 }
