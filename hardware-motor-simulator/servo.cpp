/*
 * This routine services the servo input interrupts and keeps track of
 * the servo pulse widths.
 */

#include "Arduino.h"
#include "io_ref.h"
#include "pins.h"

#define	SERVO_MIN	544UL
#define	SERVO_MAX	2400UL
#define	SERVO_ERROR	10UL
#define MIN_CHANGE	8		// minimum change that we ignore.  Min possible change is 4 uSec

volatile int input_ipa_servo;		// pulse width in microseconds
volatile int input_n2o_servo;
volatile bool ipa_servo_change;
volatile bool n2o_servo_change;

static unsigned long ipa_raise_time;
static unsigned long n2o_raise_time;
volatile bool ipa_valid;
volatile bool n2o_valid;
volatile unsigned long ipa_last_valid;
volatile unsigned long n2o_last_valid;

extern unsigned long loop_time;

// ISR for IPA servo pin change
static void ipa_isr() {
	int d;
	int c;

	if (digitalRead(PIN_MAIN_IPA)) {
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
	
	c = d - input_ipa_servo;
	if (c > MIN_CHANGE || c < -MIN_CHANGE) {
		ipa_servo_change = true;
		input_ipa_servo = d;
	}
	ipa_last_valid = micros();
}

// ISR for N2O servo pin change
static void n2o_isr() {
	int d;
	int c;

	if (digitalRead(PIN_MAIN_N2O)) {
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
	
	c = d - input_n2o_servo;
	if (c > MIN_CHANGE || c < -MIN_CHANGE) {
		n2o_servo_change = true;
		input_n2o_servo = d;
	}
	n2o_last_valid = micros();
}

void servo_setup() {
	pinMode(PIN_MAIN_IPA, INPUT);
	pinMode(PIN_MAIN_N2O, INPUT);
	ipa_valid = false;
	n2o_valid = false;
	ipa_last_valid = 0;
	n2o_last_valid = 0;
	ipa_servo_change = true;
	n2o_servo_change = true;
	attachInterrupt(digitalPinToInterrupt(PIN_MAIN_IPA), ipa_isr, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_MAIN_N2O), n2o_isr, CHANGE);
}


/*
 * Routines to read the pulse width (safely) and convert it to an angle
 * They return -1 if no recent valid pulse width, and -2 if the pulse is too wide or too narrow
 */
int servo_read_ipa() {
	int l_micros;
	unsigned long l_since;
	unsigned long v;
	unsigned long m;
	long t;

	noInterrupts();
	l_micros = input_ipa_servo;
	v = ipa_last_valid;
	interrupts();
	m = micros();
	l_since = m - v;

	if (v == 0 || l_since > 50000UL)
		return -1;

	if (l_micros < SERVO_MIN - SERVO_ERROR || l_micros > SERVO_MAX + SERVO_ERROR)
		return -2;

	if (l_micros < SERVO_MIN)
		l_micros = SERVO_MIN;

	if (l_micros > SERVO_MAX)
		l_micros = SERVO_MAX;

	t = (((long)l_micros - SERVO_MIN) * 180UL) / (SERVO_MAX-SERVO_MIN);
	return t;
}

int servo_read_n2o() {
	int l_micros;
	unsigned long l_since;
	unsigned long v;
	unsigned long m;
	long t;

	noInterrupts();
	l_micros = input_n2o_servo;
	v = n2o_last_valid;
	interrupts();
	m = micros();
	l_since = m - v;

	if (v == 0 || l_since > 50000UL)
		return -1;

	if (l_micros < SERVO_MIN - SERVO_ERROR || l_micros > SERVO_MAX + SERVO_ERROR)
		return -2;

	if (l_micros < SERVO_MIN)
		l_micros = SERVO_MIN;

	if (l_micros > SERVO_MAX)
		l_micros = SERVO_MAX;

	t = (((long)l_micros - SERVO_MIN) * 180UL) / (SERVO_MAX-SERVO_MIN);
	return t;
}
