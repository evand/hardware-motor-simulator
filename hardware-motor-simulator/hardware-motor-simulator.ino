/*
 * Hardware Motor Simulator
 * 
 * This code runs on the P2/V4.4 Hardware Motor Simulator
 * 
 * All of the important code is in .cpp files in this directory.
 * This main program simply calls the various modules.
 */

#include "log.h"
#include "state.h"
#include "menu.h"

/*
 * LCD Stuff
 */
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int backlight = 6;

unsigned long loop_time;

extern void input_setup();
extern void output_setup();
extern void full_run_init();
extern void spark_test_init();
extern void servo_setup();

void setup() {
  Serial.begin(9600);
  /*xxx*/delay(2000);Serial.print("Hello World.\n");

  state_init();
  
  // Set up the LCD backlight
  pinMode(backlight, OUTPUT);
  analogWrite(backlight, 128);  // 50% power
  
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);

  log_init();
  menu_init();
  input_setup();
  output_setup();
  servo_setup();
}

extern void inputs();
extern void outputs();

void loop() {
   /*xxx*/ //delay(300);Serial.print("x");return;
  loop_time = millis();
 
  inputs();
  state_machine();
  outputs();
}
