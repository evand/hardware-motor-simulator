/*
 * Test routine for ig valve input.
 *
 * This routine prints both ig valve states on LCD
 * Updates are limited to 10 per second, to avoid flickering.
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

void ig_valve_test_state(bool first_time) {
	if (first_time) {
/*xxx*/Serial.print("Ig Valve Test State\n"); delay(100);
		lcd.clear();
		lcd.setCursor(3, 0);
		lcd.print("Ig Valve Test");
		lcd.setCursor(0, 2);
		lcd.print("IPA Valve:");
		lcd.setCursor(0, 3);
		lcd.print("N2O Valve:");
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

	lcd.setCursor(12, 2);
	lcd.print(input_ig_valve_ipa_level? "OPEN  ": "CLOSED");

	lcd.setCursor(12, 3);
	lcd.print(input_ig_valve_n2o_level? "OPEN  ": "CLOSED");
}
