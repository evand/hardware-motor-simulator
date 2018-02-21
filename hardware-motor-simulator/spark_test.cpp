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

static unsigned long next_update_time;
extern unsigned long loop_time;
static unsigned long const update_period = 100;

void spark_test_state(bool first_time) {
/*xxx*/Serial.print("Spark Test State\n"); delay(100);
	if (first_time) {
		lcd.clear();
		lcd.setCursor(0, 3);
		lcd.print("Spark Test");
		lcd.setCursor(2, 3);
		lcd.print("Spark Sense:");
		lcd.setCursor(3, 3);
		lcd.print("Spark Value:");
		next_update_time = 0;
	}
	
	// Exit the test when the action button is pressed.
	if (input_action_button) {
		input_action_button = false;
		state_new(menu_state);
		return;
	}

	// If not read to update, done.
	if (loop_time < next_update_time)
		return;

	// schedule next update.
	next_update_time = loop_time + update_period;

	lcd.setCursor(2, 17);
	lcd.print(input_spark_sense? "PRESENT": "ABSENT");

	lcd.setCursor(3, 17);
	lcd.print(input_spark_sense_A);
}
