/*
 * This module implements the simplest possible state machine.
 * The state is represented by a pointer to a function.
 *	State function returns void
 *	State function takes one parameter, a bool, that is true on state entry.
 *
 * Entry Points:
 * 	state_machine();	Called from loop
 * 	state_new(fn);		Called to inform the machine that next time we need a new state.
 *
 * The current state function is called once each loop().  That is all.
 */

#include <Arduino.h>	// for debugging only
#include "state.h"

static void (*current_state)(bool);
static bool new_state;
static bool next_state_is_new;

void state_init() {
	current_state = 0;
	new_state = true;
	next_state_is_new = false;
}

void state_machine() {
/*xxx*/if (current_state && new_state) {Serial.print("Calling new state\n");delay(100);}
	new_state_is_new = false;
	if (current_state) 
		current_state(new_state);
	new_state = next_state_is_new;
	next_state_is_new = false;
}


void state_new(void (*state_function)(bool)) {
/*xxx*/Serial.print("Setting new state\n");
	if (state_function == current_state)
		return;

/*xxx*/Serial.print("Setting new new state\n");
	current_state = state_function;
	next_state_is_new = true;
	new_state = true;
}
