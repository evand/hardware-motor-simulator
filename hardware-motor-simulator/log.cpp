/*
 *  This module manages the log
 *
 *  The log resides in eeprom and in main memory.
 *  On initialization valid log entries in eeprom
 *  are read into main memory.
 *
 *  The log is written to eeprom using log_commit()
 *
 *  Each log entry is 4 bytes.  See log.h for details.
 *  In addition to reading and writing the log,
 *  This code also converts log entries into strings
 *  for display on LCD and printing on serial port.
 */

#include "log.h"
#include "log_op_names.h"
#include "ee.h"
#include "EEPROM.h"

extern unsigned long loop_time;
static unsigned long log_base_time;
static int n_log_entries;
struct log_entry_s log_in_memory[LOG_SIZE];	// the in-memory copy of the log
static unsigned int log_sequence_number;
static unsigned char log_sequence_increment;

/*
 * Read the log from EEPROM, if EEPROM is available
 */
void log_init() {
	int cb2;
	int i;
	int p;

	log_sequence_number = EEPROM.read(LOG_SEQ0);
	log_sequence_number |= ((unsigned int)(EEPROM.read(LOG_SEQ1)) << 8);
	log_sequence_increment = 1;

	n_log_entries = EEPROM.read(LOG_CHECK_BYTE_1);
	cb2 = EEPROM.read(LOG_CHECK_BYTE_2);

	// check if log in eeprom is valid.
	if (n_log_entries + cb2 != LOG_SIZE ||
	    n_log_entries <= 0 ||
	    n_log_entries >= LOG_SIZE) {
		n_log_entries = 0;
		return;
	}

	p = LOG_BASE;
	for (i = 0; i < n_log_entries; i++) {
		log_in_memory[i].log_op = EEPROM.read(p++);
		log_in_memory[i].log_param = EEPROM.read(p++);
		log_in_memory[i].timestamp = EEPROM.read(p++);
		log_in_memory[i].timestamp |= (EEPROM.read(p++) << 8);
	}
}

void log_commit() {
	int cb2;
	int i;
	int p;

	// make the log invalid
	cb2 = EEPROM.read(LOG_CHECK_BYTE_2);
	if (cb2 + n_log_entries == LOG_SIZE)
		EEPROM.write(LOG_CHECK_BYTE_2, LOG_SIZE + 1);

	EEPROM.write(LOG_CHECK_BYTE_1, n_log_entries);
	p = LOG_BASE;
	for (i = 0; i < n_log_entries; i++) {
		EEPROM.write(p++, log_in_memory[i].log_op);
		EEPROM.write(p++, log_in_memory[i].log_param);
		EEPROM.write(p++, (log_in_memory[i].timestamp) & 0xff);
		EEPROM.write(p++, (log_in_memory[i].timestamp >> 8) & 0xff);
	}
	EEPROM.write(LOG_CHECK_BYTE_2, LOG_SIZE - n_log_entries);

	EEPROM.write(LOG_SEQ0, 0xff & log_sequence_number);
	EEPROM.write(LOG_SEQ1, 0xff & (log_sequence_number >> 8));
	log_sequence_increment = 1;
}

/*
 * Erase the in-memory log
 */
void log_reset() {
	n_log_entries = 0;
	log_sequence_number += log_sequence_increment;
	log_sequence_increment = 0;
}

static void i_log(unsigned char op, unsigned char param)  {
	log_in_memory[n_log_entries].log_op = op;
	log_in_memory[n_log_entries].log_param = param;
	log_in_memory[n_log_entries].timestamp = loop_time;
	n_log_entries++;
}

void log(unsigned char op, unsigned char param) {
	int max;

	switch(LOG_LEVEL(op)) {
		case LOG_CRITICAL:
			max = LOG_SIZE;
			break;
		case LOG_NORMAL:
			max = LOG_MAX_NORMAL;
			break;
		default:
			max = LOG_MAX_DETAIL;
			break;
	}

	if (n_log_entries >= max)
		return;
	
	if (n_log_entries == 0) {
		log_base_time = loop_time;
		i_log(LOG_START, 0);
	}

	// process time stamp rollover
	while (loop_time - log_base_time > LOG_ROLLOVER_TIME) {
		if (n_log_entries < LOG_SIZE - 1)
			i_log(LOG_TIME_ROLLOVER, 0);
		log_base_time += LOG_ROLLOVER_TIME;
	}
	i_log(op, param);
}

/*
 * These routines get information out of the log in the form of printable strings.
 * Notes:
 * 	All of these routines return a pointer to the same static output buffer.
 * 	Strings are null terminated, and do not have a newline.
 * 	All strings are 20 characters wide, or less, except for routines with "long"
 * 	in their name.
 */

#define	LOG_STRING_SIZE		40
#define	LOG_STRING_SHORT	20

static char log_string[LOG_STRING_SIZE];

static void i_zip() {
	char i;

	for (i = 0; i < LOG_STRING_SIZE-1; i++)
		log_string[i] = ' ';
	log_string[LOG_STRING_SIZE-1] = '\0';
}

/*
 * Print an unsigned int into the log.
 * 4 digits, fixed width, leading zeros suppressed
 */
static void i_print_n_i(char col, int val) {
	char i;

	for (i = 3; i >= 0; i--) {
		log_string[col + i] = '0' + (val % 10);
		val /= 10;
		if (val <= 0)
			break;
	}
}

/*
 * Print an unsigned char into the log.
 * 3 digits, fixed width, leading zeros suppressed
 */
static void i_print_n_c(char col, unsigned char val) {
	char i;

	for (i = 2; i >= 0; i--) {
		log_string[col + i] = '0' + (val % 10);
		val /= 10;
		if (val <= 0)
			break;
	}
}

/*
 * Very Dangerous String Copy routine
 */
static void i_strcpy(char *p, const char *q) {
	while (*p++ = *q++) ;
}

/*
 * Return the log sequence number as a printable string
 */
char *log_tos_seqn() {
	i_zip();

	i_strcpy(log_string, "Log #:");
	i_print_n_i(7, log_sequence_number);

	log_string[12] = '\0';
	return log_string;
}

/*
 * Put the timestamp into the long string
 */
static unsigned char i_log_tos(unsigned char entry) {
	char bias;
	char i;

	i_zip();
	if (entry >= n_log_entries) {
		log_string[20] = '\0';
		return 1;
	}

	bias = 0;
	for (i = 0; i < entry; i++)
		if (log_in_memory[i].log_op == LOG_TIME_ROLLOVER)
			bias++;

	if (bias > 9)
		log_string[0] = '*';
	else if (bias)
		log_string[0] = '0' + bias;
	i_print_n_i(1, log_in_memory[entry].timestamp + (bias? 10000: 0));

	return 0;
}

static void i_opcode_print(const char * const table[], unsigned char op) {
	// Necessary casts and dereferencing, just copy.
	strcpy_P(log_string + 6, (char*)pgm_read_word(&(table[op])));
}

/*
 * Return a log entry as a printable string, exactly 20 characters long
 * Format:
 *  nnnnn_ssssssssss_ppp
 * Where nnnnn is the time stamp
 *       ssssssssss is the opcode as a sting
 *       ppp is the parameter
 */
char *log_tos_short(unsigned char entry) {
	unsigned char p;

	if (i_log_tos(entry))
		return log_string;
	log_string[20] = '\0';

	i_opcode_print(op_codes_short, (log_in_memory[entry].log_op) & ~LOG_LEVEL_MASK);

	p = log_in_memory[entry].log_param;
	if (p)
		i_print_n_c(17, p);

	return log_string;
}

/*
 * Return a log entry as a printable string, exactly 30 characters long
 * Format:
 *  nnnnn_ssssssssss_ppp
 * Where nnnnn is the time stamp
 *       ssssssssssssssssssss is the opcode as a sting
 *       ppp is the parameter
 */
char *log_tos_long(unsigned char entry) {
	unsigned char p;

	if (i_log_tos(entry))
		return log_string;
	log_string[30] = '\0';

	i_opcode_print(op_codes_long, (log_in_memory[entry].log_op) & ~LOG_LEVEL_MASK);

	p = log_in_memory[entry].log_param;
	if (p)
		i_print_n_c(27, p);

	return log_string;
}
