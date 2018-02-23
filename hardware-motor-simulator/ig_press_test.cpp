/*
 * Test routine for ig pressure input.
 *
 * This routine prints both the raw and scaled ig pressure sensor value
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "io_ref.h"
#include "state.h"
#include "menu.h"
#include "buffer.h"
#include "dac.h"

extern LiquidCrystal lcd;

static unsigned long next_update_time;
extern unsigned long loop_time;
static unsigned long const update_period = 100;

void ig_press_test_state(bool first_time) {
	long c;

/*xxx*/Serial.print("Ig Pressure Test\n"); delay(100);
	if (first_time) {
		lcd.clear();
		lcd.setCursor(0, 3);
		lcd.print("Ig Pressure Test");
		lcd.setCursor(2, 0);
		lcd.print("   Raw Value:");
		lcd.setCursor(3, 0);
		lcd.print("Scaled Value:");
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

	buffer_zip();
	buffer[6] = '\0';
	lcd.setCursor(2, 14);
	lcd.print(buffer);
	lcd.setCursor(3, 14);
	lcd.print(buffer);

	if (dac_ig_press_present()) {
		lcd.setCursor(2, 14);
		lcd.print(input_ig_press);

		c = input_ig_press - 500;
		if (c < 0)
			c = 0;
		c = (c * 32768L) / 5000L;
		lcd.setCursor(3, 14);
		lcd.print(c);
	} else {
		lcd.setCursor(2, 14);
		lcd.print("N/A");
	}
}
