/*
 * Do The Thing
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "io_ref.h"
#include "state.h"
#include "menu.h"
#include "log.h"

extern LiquidCrystal lcd;

void full_run_state(bool first_time) {
/*xxx*/Serial.print("Full Run State\n"); delay(1000);
	if (first_time) {
		lcd.clear();
		log_reset();
		log_enabled = true;
	}
	
	if (input_action_button) {
		input_action_button = false;
		log_commit();
		log_enabled = false;
		state_new(menu_state);
	}
}
