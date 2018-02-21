/*
 * This routine services the servo input interrupts and keeps track of
 * the servo pulse widths.
 */

#include "Arduino.h"
#include "io_ref.h"

// Input Pins
static const int ipa_servo = 2;
static const int n2o_servo = 3;

volatile int input_ipa_servo;		// pulse width in microseconds
volatile int input_n2o_servo;

static long ipa_raise_time;
static long n2o_raise_time;
static bool ipa_valid;
static bool n2o_valid;
static long ipa_last_valid;
static long n2o_last_valid;

extern int loop_time;

// ISR for IPA servo pin change
static void ipa_isr() {
	int d;

	if (digitalRead(ipa_servo)) {
		ipa_raise_time = micros();
		ipa_valid = true;
		return;
	}

	if (!ipa_valid)
		return;

	d = micros() - ipa_raise_time;
	ipa_valid = false;
	if (d <= 0)
		return;

	input_ipa_servo = d;
	ipa_last_valid = micros();
}

// ISR for N2O servo pin change
static void n2o_isr() {
	int d;

	if (digitalRead(n2o_servo)) {
		n2o_raise_time = micros();
		n2o_valid = true;
		return;
	}

	if (!n2o_valid)
		return;

	d = micros() - n2o_raise_time;
	n2o_valid = false;
	if (d <= 0)
		return;

	input_n2o_servo = d;
	n2o_last_valid = micros();
}

void servo_setup() {
	pinMode(ipa_servo, INPUT);
	pinMode(n2o_servo, INPUT);
	ipa_valid = false;
	n2o_valid = false;
	ipa_last_valid = 0;
	n2o_last_valid = 0;
	attachInterrupt(digitalPinToInterrupt(ipa_servo), ipa_isr, CHANGE);
	attachInterrupt(digitalPinToInterrupt(n2o_servo), n2o_isr, CHANGE);
}


/*
 * Routines to read the pulse width (safely) and convert it to an angle
 * They return -1 if no recent valid pulse width, and -2 if the pulse is too wide or too narrow
 */
int servo_read_ipa() {
	int l_micros;
	int l_since;

	l_since = loop_time - ipa_last_valid;

	if (ipa_last_valid == 0 || l_since < 0 || l_since > 50)
		return -1;

	noInterrupts();
	l_micros = input_ipa_servo;
	interrupts();

	if (l_micros < 500 || l_micros > 1500)
		return -2;

	// (l_micros - 500) * 180/1000
	return (l_micros - 500) * 9 / 50;
}

int servo_read_n2o() {
	int l_micros;
	int l_since;

	l_since = loop_time - n2o_last_valid;

	if (n2o_last_valid == 0 || l_since < 0 || l_since > 50)
		return -1;

	noInterrupts();
	l_micros = input_n2o_servo;
	interrupts();

	if (l_micros < 500 || l_micros > 1500)
		return -2;

	// (l_micros - 500) * 180/1000
	return (l_micros - 500) * 9 / 50;
}
