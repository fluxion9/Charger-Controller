int chargePin = 1; // I/O 3, connects to the gate of the switching pMOSFET.
int input = A2; // A2, connects to the voltage divider at the input side.
int battery = A3; // A3, connects to the voltage divider at the output side.
int cIndicator = 0;
int cfIndicator = 2;
float chargeVoltage = 13.55; // change this to the voltage of your battery at full charge.
float minimumChargingVoltage = 13.0; // change this to the lowest voltage required to charge your battery
float maximumChargingVoltage = 15.5; // change this to the highest voltage required to charge your battery.
float maximumChargerVoltage = 100.0; // change this to the highest voltage you want to give the charge controller's input.
float maxBattery = 100.0; 
float lowVoltage = 10.5; // change this to the voltage of your battery when it is low.

void setup() {
  pinMode(chargePin, OUTPUT);
  pinMode(cIndicator, OUTPUT);
  pinMode(cfIndicator, OUTPUT);
  digitalWrite(cIndicator, 1);
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(cfIndicator, 1);
    delay(300);
    digitalWrite(cfIndicator, 0);
    delay(300);
  }
  digitalWrite(cIndicator, 0);
  digitalWrite(cfIndicator, 0);
  TCCR0B = TCCR0B & B11111000 | B00000001;
  analogWrite(chargePin, 0);
}

void loop() {
  digitalWrite(chargePin, LOW);
  float inputVoltage = measureVoltage(input);
  float batteryVoltage = measureVoltage(battery);
  if(batteryVoltage >= chargeVoltage)
  {
    digitalWrite(chargePin, LOW);
    digitalWrite(cIndicator, LOW);
    digitalWrite(cfIndicator, HIGH);
  }
  else if(inputVoltage >= minimumChargingVoltage && inputVoltage <= maximumChargingVoltage && batteryVoltage < chargeVoltage)
  {
    digitalWrite(chargePin, HIGH);
    digitalWrite(cIndicator, HIGH);
    digitalWrite(cfIndicator, LOW);
  }
  else if(inputVoltage > maximumChargingVoltage && batteryVoltage < chargeVoltage)
  {
    int dutyCycle = (255.0 * 100.0) / inputVoltage;
    dutyCycle = constrain(dutyCycle, 0, 254);
    analogWrite(chargePin, dutyCycle);
    digitalWrite(cIndicator, HIGH);
    digitalWrite(cfIndicator, LOW);
  }
  else {
    digitalWrite(chargePin, LOW);
    digitalWrite(cIndicator, HIGH);
    digitalWrite(cfIndicator, HIGH);
  }
}

float measureVoltage(int pin)
{
  float ADCval = analogRead(pin);
  float voltage = (ADCval * 100.0) / 1023.0;
  return voltage;
}
