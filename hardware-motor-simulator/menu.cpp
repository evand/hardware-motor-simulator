/*
 * This code implements the top level menu.
 */

#include <Arduino.h>
#include "state.h"
#include "io_ref.h"
#include <LiquidCrystal.h>
#include "avr/pgmspace.h"
extern LiquidCrystal lcd;

/*
 * Each menu item is a string of up to 18 characters
 * and a state function.
 *
 * This code allows users to scroll around the menu and select
 * an item.  Then the screen is blanked and the state function
 * is set to the associated state function.
 */

/*
 * Here are the menu item strings.
 */

const char  m_0[] PROGMEM = "Full Run";
const char  m_1[] PROGMEM = "Spark Test";

const char * const menu_table[] PROGMEM = {
		m_0,
		m_1,
};

/*
 * Here are the menu state actions
 */
extern void (*full_run_state)(bool);
extern void (*spark_test_state)(bool);

void (*menu_state_functions[])(bool) = {
	full_run_state,
	spark_test_state,
};

#define	N_MENU_ITEMS	2

static char menu_selection;	// which is the current menu item?

static char print_buffer[21];

/*
 * Clear the screen and then draw the menu.
 */
static void i_draw_menu()
{
	char min, max;
	char i, j;

	lcd.clear();

	min = 0;
	if (menu_selection > 0)
		min = menu_selection - 1;
	max = N_MENU_ITEMS - 1;
	if (menu_selection < max)
		max = menu_selection + 1;

	for (i = 0; i < 2; i++) {
		if (min + i > max)
			break;
		for (j = 0; j < 20; j++)
			print_buffer[j] = ' ';
		print_buffer[j] = '\0';
		// highlight the current menu selection
		if (i + min == menu_selection)
			print_buffer[0] = '*';
		strcpy_P(print_buffer + 2, (char*)pgm_read_word(&(menu_table[i + min])));
		lcd.setCursor(0, i);
		lcd.print(print_buffer);
	}
}

/*
 * Called from loop to do all the work.
 */
void menu_state(bool first_time) {
	// If the action button has been hit, then switch states.
	if (input_action_button) {
		input_action_button = false;
		/*xxx*/Serial.print("Selecting menu item "); {Serial.println((int)menu_selection); delay(500);}

		state_new(menu_state_functions[menu_selection]);
		return;
	}

	// If the scroll up button has been hit, change menu state and force redraw
	if (input_scroll_up) {
		if (menu_selection > 0) {
			menu_selection--;
			first_time = true;
		}
		input_scroll_up = false;
	}

	// If the scroll down button has been hit, change menu state and force redraw
	if (input_scroll_down) {
		if (menu_selection < N_MENU_ITEMS-1) {
			menu_selection++;
			first_time = true;
		}
		input_scroll_down = false;
	}

	if (first_time)
		i_draw_menu();
}

void menu_init() {
	menu_selection = 0;
	state_new(menu_state);
}
