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
#include "ee.h"
#include "EEPROM.h"

extern unsigned long loop_time;
static unsigned long log_base_time;
static int n_log_entries;
static struct log_entry_s log[LOG_SIZE];	// the in-memory copy of the log

/*
 * Read the log from EEPROM, if EEPROM is available
 */
void log_init() {
	int cb2;
	int i;
	int p;

	n_log_entries == EEPROM.read(LOG_CHECK_BYTE_1);
	cb2 == EEPROM.read(LOG_CHECK_BYTE_2);

	// check if log in eeprom is valid.
	if (n_log_entries + cb2 != LOG_SIZE ||
	    n_log_entries <= 0 ||
	    n_log_entries >= LOG_SIZE) {
		n_log_entries = 0;
		return;
	}

	p = LOG_BASE;
	for (i = 0; i < n_log_entries; i++) {
		log[i].log_op = EEPROM.read(p++);
		log[i].log_param = EEPROM.read(p++);
		log[i].time_stamp = EEPROM.read(p++);
		log[i].time_stamp |= (EEPROM.read(p++) << 8);
	}
}

void log_commit() {
	int cb2;
	int i;
	int p;

	// make the log invalid
	cb2 == EEPROM.read(LOG_CHECK_BYTE_2);
	if (cb2 + n_log_entries == LOG_SIZE)
		EEPROM.write(LOG_CHECK_BYTE_2, LOG_SIZE + 1);

	EEPROM.write(LOG_CHECK_BYTE_1, n_log_entries);
	p = LOG_BASE;
	for (i = 0; i < n_log_entries; i++) {
		EEPROM.write(p++, log[i].log_op);
		EEPROM.write(p++, log[i].log_param);
		EEPROM.write(p++, (log[i].time_stamp) & 0xff);
		EEPROM.write(p++, (log[i].time_stamp >> 8) & 0xff);
	}
	EEPROM.write(LOG_CHECK_BYTE_2, LOG_SIZE - n_log_entries);
}

/*
 * Erase the in-memory log
 */
void log_reset()
{
	n_log_entries = 0;
}

void log(unsigned char op, unsigned char param) 
{
	int max;

	switch(LOG_LEVEL(op)) {
		case LOG_CRITICAL:
			max = LOG_MAX;
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
	while (loop_time - log_base_time > 65536UL) {
		if (n_log_entries < LOG_MAX - 1)
			i_log(LOG_TIME_ROLLOVER, 0);
		log_base_time += 65536UL;
	}
	i_log(op, param);
}
