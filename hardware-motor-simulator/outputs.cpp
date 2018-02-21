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

// The output
unsigned char output_led;

extern unsigned long loop_time;
static unsigned long shot_time;

static const int led_pin = 13;

static const int shot_width = 100;	// Single blink is 100 ms
static const int blink_period = 500;	// 250 ms on, 250 ms off, repeat.

static void i_led() {
	switch (output_led) {
		default:
		case LED_OFF:
			digitalWrite(led_pin, LOW);
			break;
		case LED_ONE_SHOT:
			output_led = LED_CONTINUE;
			shot_time = loop_time + shot_width;
			// note fall thru
		case LED_ON:
			digitalWrite(led_pin, HIGH);
			break;
		case LED_BLINKING:
			if ((loop_time % blink_period) < blink_period/2)
				digitalWrite(led_pin, HIGH);
			else
				digitalWrite(led_pin, LOW);
			break;
		case LED_CONTINUE:
			if (loop_time >= shot_time) {
				output_led = LED_OFF;
				digitalWrite(led_pin, LOW);
			}
			break;
	}
}

void output_setup() {
	pinMode(led_pin, OUTPUT);
	output_led = LED_OFF;
	shot_time = 0;
}

void outputs() {
	i_led();
}
