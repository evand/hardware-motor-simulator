/*
 * Display the log on the LCD screen
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "io_ref.h"
#include "state.h"
#include "menu.h"
#include "log.h"

extern LiquidCrystal lcd;

static int lr_min;

void log_review_state(bool first_time) {
	int i;
	char *p;

/*xxx*/Serial.print("Spark Test State\n"); delay(100);
	if (first_time) {
		lr_min = -1;
	}
	
	// Exit the test when the action button is pressed.
	if (input_action_button) {
		input_action_button = false;
		state_new(menu_state);
		return;
	}

	if (input_scroll_up) {
		input_scroll_up = false;
		lr_min--;
		if (lr_min < -1) {
			lr_min = -1;
			first_time = true;
		}
	}

	if (input_scroll_down) {
		input_scroll_down = false;
		if (*log_tos_short(lr_min+3)) {
			lr_min++;
			first_time = true;
		}
	}

	if (!first_time)
		return;

	// draw the screen
	for (i = 0; i < 4; i++) {
		lcd.setCursor(0, i);
		if (i + lr_min < 0)
			p = log_tos_seqn();
		else
			p = log_tos_short(i + lr_min);
		if (*p)
			lcd.print(p);
		else
			lcd.print("                    ");
	}
}
