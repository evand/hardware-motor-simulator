/*
 * Handle some of the output processing.
 * DOES NOT handle the LCD screen.  Various bits of code do that whereever.
 * DOES NOT handle the DAC outputs.  Those are directly manipulated by the code.
 *
 * Only one output handled here:
 * Status LED
 * 	This is the green LED below the scroll switch.
 * 	This routine sets it according to a state variable
 * 	that anybody can manipulate.
 */

#include "Arduino.h"
#include "io_ref.h"
#include "pins.h"

// The output
unsigned char output_led;

extern unsigned long loop_time;
static unsigned long shot_time;

static const int shot_width = 100;	// Single blink is 100 ms
static const int blink_period = 500;	// 250 ms on, 250 ms off, repeat.

static void i_led() {
	switch (output_led) {
		default:
		case LED_OFF:
			digitalWrite(PIN_LED, LOW);
			break;
		case LED_ONE_SHOT:
			output_led = LED_CONTINUE;
			shot_time = loop_time + shot_width;
			// note fall thru
		case LED_ON:
			digitalWrite(PIN_LED, HIGH);
			break;
		case LED_BLINKING:
			if ((loop_time % blink_period) < blink_period/2)
				digitalWrite(PIN_LED, HIGH);
			else
				digitalWrite(PIN_LED, LOW);
			break;
		case LED_CONTINUE:
			if (loop_time >= shot_time) {
				output_led = LED_OFF;
				digitalWrite(PIN_LED, LOW);
			}
			break;
	}
}

void output_setup() {
	pinMode(PIN_LED, OUTPUT);
	output_led = LED_OFF;
	shot_time = 0;
}

void outputs() {
	i_led();
}
