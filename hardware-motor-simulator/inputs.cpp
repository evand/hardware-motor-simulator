/*
 * Handle all the input processing.
 * All of the event-type inputs are edge triggered.
 * 	These include the action button, the scroll switch,
 * 	and the two solenoid valves.
 *
 * 	When they are actuated the boolean event variable is set true.
 * 	The event consumer must set the event variable false when
 * 	done consuming the event, otherwise subsequent events are ignored.
 *
 * 	The action button and the scroll switch are debounced, so the
 * 	event is delayed by 10 ms, and on-times of less than 10 ms are
 * 	ignored.
 *
 *	For now the solenoids are not debounced.  Event logging code
 *	Will have to deal with this fact.  Or maybe the relays don't bounce.
 *
 * The two analog inputs are raw 10-bit values.
 *	These values are not filtered, we do add some simple hysterisis.
 *	If the value read off the input A/D has not changed by +/- 3, we
 *	don't change the reported value.  This should provide some simple
 *	noise filtering.
 *
 * The spark sensor input is converted to a digital value, but
 * 	the analog value is available too.  The digital value is neither
 * 	debounced nor edge triggered.  It is set both true and false here.
 *
 * NOTE:
 * 	Various thresholds are hard coded into the input routines.
 * 	This simplifies the code by not cluttering things with a bunch
 * 	of #defines that are used exactly once.  Read the code if you
 * 	want to see the thresholds.
 * 	This applies to scroll switch and spark sense inputs.
 */

#include "Arduino.h"
#include "io_ref.h"

extern unsigned long loop_time;

static const int action_button = A3;
static const int scroll_switch = A6;
static const int ig_valve_ipa = 5;	// QQQ XXX NOTE: These are almost certainly wrong.
static const int ig_valve_n2o = 6;
static const int main_press = A2;
static const int ig_press = A3;
static const int spark_sense = A1;

// these are the events.  They are set true here, and set
// false when consumed by the consumer
bool input_action_button;
bool input_scroll_up;
bool input_scroll_down;
bool input_ig_valve_ipa;
bool input_ig_valve_n2o;
bool input_spark_sense;
int  input_spark_sense_A;	// analog value.  Used only in test routines.

// These are the analog input values.  They are set here only.
int  input_main_press;
int  input_ig_press;

// Action button variables
static bool action_button_debounce;
static bool action_button_old_state;
static unsigned long action_button_debounce_time;

// Scroll switch variables
static bool scroll_debounce;
static unsigned char scroll_old_state;
static unsigned long scroll_debounce_time;

// Ig Valve variables
static bool ig_valve_ipa_old_state;
static bool ig_valve_n2o_old_state;

// Pressor sensor variables
// (none needed)

const static unsigned long debounce_time = 10;	// milliseconds
const static int hysterisis = 3;		// counts

static void i_action_button() {
	boolean v;

	// v is true if the button is pressed
	v = (digitalRead(action_button) == 0);
	
	// Rising edge?
	if (v && !action_button_old_state) {
		action_button_debounce = true;
		action_button_debounce_time = loop_time + debounce_time;
	// True and debounce period over?
	} else if (v && action_button_debounce &&
			loop_time > action_button_debounce_time) {
		input_action_button = true;
		action_button_debounce = false;
	}

	// If button not pressed cancel any pending debounced rising edge
	if (!v)
		action_button_debounce = false;

	action_button_old_state = v;
}

static void i_scroll_switch() {
	int t;
	unsigned char v;

	// v is true if switch pressed either way
	t = analogRead(scroll_switch);
	if (t < 10)
		v = 1;
	else if (t > 1000)
		v = 2;
	else
		v = 0;

	// Rising edge?
	if (v && v != scroll_old_state) {
		scroll_debounce = true;
		scroll_debounce_time = loop_time + debounce_time;
	// Debounce period over?
	} else if (v && scroll_debounce &&
			loop_time > scroll_debounce_time) {
		if (v == 1)
			input_scroll_up = true;
		else
			input_scroll_down = true;
		scroll_debounce = false;
	}

	// If the switch is center-off cancel any pending debounce1G
	if (!v)
		scroll_debounce = false;
	scroll_old_state = v;
}

static void i_ig_valve_ipa() {
	boolean v;

	// v is true if the solenoid is actuated
	v = (digitalRead(ig_valve_ipa) == 0);

	if (v && !ig_valve_ipa_old_state)
		input_ig_valve_ipa = true;
	ig_valve_ipa_old_state = v;
}

static void i_ig_valve_n2o() {
	boolean v;

	// v is true if the solenoid is actuated
	v = (digitalRead(ig_valve_n2o) == 0);

	if (v && !ig_valve_n2o_old_state)
		input_ig_valve_n2o = true;
	ig_valve_n2o_old_state = v;
}

static void i_main_press() {
	int v, t;

	v = analogRead(main_press);
	t = v - input_main_press;
	if (t >= hysterisis || t <= -hysterisis)
		input_main_press = v;
}

static void i_ig_press() {
	int v, t;

	v = analogRead(ig_press);
	t = v - input_ig_press;
	if (t >= hysterisis || t <= -hysterisis)
		input_ig_press = v;
}

static void i_spark_sense() {
	input_spark_sense_A = analogRead(spark_sense);
	input_spark_sense = (input_spark_sense_A < 300? false: true);
}

void input_setup() {
	pinMode(action_button, INPUT_PULLUP);
	action_button_old_state = false;
	input_action_button = false;

	pinMode(scroll_switch, INPUT);
	scroll_old_state = 0;
	input_scroll_up = false;
	input_scroll_down = false;

	pinMode(ig_valve_ipa, INPUT);
	ig_valve_ipa_old_state = false;
	input_ig_valve_ipa = false;

	pinMode(ig_valve_n2o, INPUT);
	ig_valve_n2o_old_state = false;
	input_ig_valve_n2o = false;

	pinMode(main_press, INPUT);
	input_main_press = 0;

	pinMode(ig_press, INPUT);
	input_ig_press = 0;

	pinMode(spark_sense, INPUT);
	input_spark_sense = 0;
	input_spark_sense_A = 0;
}

void inputs() {
	i_action_button();
	i_scroll_switch();
	i_ig_valve_ipa();
	i_ig_valve_n2o();
	i_main_press();
	i_ig_press();
	i_spark_sense();
}
