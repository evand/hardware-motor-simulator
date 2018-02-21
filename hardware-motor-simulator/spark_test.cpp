/*
 * Test routine for spark input.
 *
 * This routine prints both the analog and digital value of the spark state
 * on the LCD.   Updates are limited to 10 per second, to avoid flickering.
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "io_ref.h"
#include "state.h"
#include "menu.h"

extern LiquidCrystal lcd;

void spark_test_state(bool first_time) {
/*xxx*/Serial.print("Spark Test State\n"); delay(1000);
	if (first_time)
		lcd.clear();
	
	// Exit the test when the action button is pressed.
	if (input_action_button) {
		input_action_button = false;
		state_new(menu_state);
		return;
	}
}
