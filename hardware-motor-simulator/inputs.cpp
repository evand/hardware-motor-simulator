/*
 * Handle all the input processing.
 */

#include "Arduino.h"
#include "io_ref.h"

extern unsigned long loop_time;

static const int action_button = A3;
static const int scroll_switch = A6;

// these are the events.  They are set true here, and set
// false when consumed
bool input_action_button;
bool input_scroll_up;
bool input_scroll_down;

bool action_button_debounce;
static unsigned long action_button_debounce_time;

bool scroll_debounce;
static unsigned long scroll_debounce_time;

const static unsigned long debounce_time = 5;	// milliseconds

static void i_action_button() {
	boolean v;

	// v is true if the button is pressed
	v = (digitalRead(action_button) == 0);
	
	// If they let up on the button before action consumed, discard action
	if (!v && input_action_button) {
		input_action_button = false;
		return;
	}

	// If the button is still down before action is consumed, do nothing
	if (v && input_action_button)
		return;

	// If we are debouncing a button press
	if (action_button_debounce) {
		// If they let up on the button, discard
		if (!v) {
			action_button_debounce = false;
			return;
		}
		if (action_button_debounce_time > loop_time) {
			action_button_debounce = false;
			input_action_button = true;
			return;
		}
		// keep waiting for debounce period
		return;
	}

	// If not debouncing, but button is down, start debouncing
	if (v) {
		action_button_debounce = true;
		action_button_debounce_time = loop_time + debounce_time;
		return;
	}

	// not debounce, button down, do nothing.
	return;
}

static void i_scroll_switch() {
	boolean v_up, v_down;
	int v;

	// v is true if the button is pressed
	v = analogRead(scroll_switch);
	v_up = (v < 10? true: false);
	v_down = (v > 1000? true: false);
	
	// If they let up on the button before action consumed, discard action
	if (!v_down && input_scroll_down) {
		input_scroll_down = false;
		return;
	}
	if (!v_up && input_scroll_up) {
		input_action_button = false;
		return;
	}

	// If the button is still down before action is consumed, do nothing
	if (v_up && input_scroll_up)
		return;
	if (v_down && input_scroll_down)
		return;

	// If we are debouncing a button press
	if (scroll_debounce) {
		// If they let up on the button, discard
		if (!v_up && !v_down) {
			scroll_debounce = false;
			return;
		}
		if (scroll_debounce_time > loop_time) {
			scroll_debounce = false;
			if (v_up)
				input_scroll_up = true;
			if (v_down)
				input_scroll_down = true;
			return;
		}
		// keep waiting for debounce period
		return;
	}

	// If not debouncing, but button is down, start debouncing
	if (v_up || v_down) {
		scroll_debounce = true;
		scroll_debounce_time = loop_time + debounce_time;
		return;
	}

	return;
}

void input_setup() {
	pinMode(action_button, INPUT_PULLUP);
	pinMode(scroll_switch, INPUT);
}

void inputs() {
	i_action_button();
	i_scroll_switch();
}
