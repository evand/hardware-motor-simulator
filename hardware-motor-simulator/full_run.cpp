/*
 * Do The Thing
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "io_ref.h"
#include "state.h"
#include "menu.h"

extern LiquidCrystal lcd;

void full_run_state(bool first_time) {
/*xxx*/Serial.print("Full Run State\n"); delay(1000);
	if (first_time)
		lcd.clear();
	
	if (input_action_button) {
		input_action_button = false;
		state_new(menu_state);
	}
}
