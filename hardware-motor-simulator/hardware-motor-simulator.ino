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
#include "pins.h"

/*
 * LCD Stuff
 */
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

unsigned long loop_time;
unsigned long loop_counter;

extern void input_setup();
extern void output_setup();
extern void full_run_init();
extern void spark_test_init();
extern void servo_setup();
extern void dac_setup();

void setup() {
  Serial.begin(9600);
  /*xxx*/delay(2000);Serial.print("Hello World.\n");

  state_init();
  
  // Set up the LCD backlight
  pinMode(PIN_BACKLIGHT, OUTPUT);
  analogWrite(PIN_BACKLIGHT, 128);  // 50% power
  
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  loop_counter = 0;

  log_init();
  menu_init();
  input_setup();
  output_setup();
  servo_setup();
  dac_setup();
}

extern void inputs();
extern void outputs();

void loop() {
  loop_time = millis();
  loop_counter++;
 
  inputs();
  state_machine();
  outputs();
}
