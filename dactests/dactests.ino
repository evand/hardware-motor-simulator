
#include <Wire.h>

// This sketch writes some data to the Sparkfun DAC
//
// This is intended as part of the bring-up of the P2 Motor Simulator hardware
//

#define DAC_ADDR  0x61  // https://www.sparkfun.com/products/12918
#define LED 13

// Init the DAC to zero volts.  Set DAC to send zero volts on power up.
void dac_init() {
  //return;
  Wire.beginTransmission(DAC_ADDR);
  Wire.write(0x60);           // cmd to update the DAC and DAC EEPROM
  Wire.write(0);              // the 8 most significant bits...
  Wire.write(0);              // the 4 least significant bits...
  Wire.endTransmission();
}

// This routine sets the output voltage on the DAC using a 12-bit number.
// This does not affect the power-up voltage of the DAC
// 0 = 0V.
// 4095 = VCC (5.0 volts)

void dac_set(int val) {
  //return;
  Wire.beginTransmission(DAC_ADDR);
  Wire.write(0x40);                  // cmd to update the DAC
  Wire.write(val >> 4);              // the 8 most significant bits...
  Wire.write((val & 0x0f) << 4);     // the 4 least significant bits...
  Wire.endTransmission();
}

boolean prompted;

void setup() {
  
  Serial.begin(9600);
  delay(1000);
  Serial.print("Enter desired output voltage (0.0 to 5.0)");
  prompted = true;
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  Wire.begin();
  dac_init();
 digitalWrite(LED, LOW);
}

void loop() {
  float v;
  int d;

  if ((millis()/100) % 2)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);  

  if (!prompted) {
    Serial.print("? ");
    prompted = true;
  }
  if (!Serial.available()) {
    return;
  }
  
  v = Serial.parseFloat();
  delay(100);
  while (Serial.read() >= 0)
    ;
  prompted = false;
  
  if (v < 0.) {
    Serial.print("value less than 0.\n");
  } else if (v > 5.) {
    Serial.print("value greater than 5.0\n");
  } else {
    d = v * 4095. / 5.;
    if (d > 4095)
      d = 4095;
    dac_set(d);
    Serial.print("Sent ");
    Serial.print(d);
    Serial.print("\n");
  }
}
