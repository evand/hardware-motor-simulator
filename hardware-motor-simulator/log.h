/*
 * Defines the log
 */

/*
 * NOTE: code assume that LOG_SIZE+1 fits in an 8-bit integer
 */
#define	LOG_SIZE	100	// no more than 100 entries are stored
#define	LOG_MAX_NORMAL	LOG_SIZE - 10
#define	LOG_MAX_DETAIL	LOG_MAX_NORMAL - 10

struct log_entry_s {
	unsigned char log_op;		// see various #defines
	unsigned char log_param;	// some op codes take a numeric parameter
	unsigned int time_stamp;	// Time when the logged event happened.
};

/*
 * Notes on log time stamps
 *
 * The first log entry always happens at time t=0.
 * All other log events happen at some number of milliseconds since t=0.
 * If the log runs more than 65 seconds, a log rollover event is recorded.
 */

/*
 * Log Levels:
 * 	Three levels of logging allows for users to see only the
 * 	important subsets.  Also, last few entries in the log are
 * 	reserved for important log entries.
 */

#define	LOG_LEVEL_SHIFT	6
#define	LOG_CRITIAL	(0 << LOG_LEVEL_SHIFT)
#define	LOG_NORMAL	(1 << LOG_LEVEL_SHIFT)
#define	LOG_DETAIL	(2 << LOG_LEVEL_SHIFT)
#define	LOG_LEVEL_MASK	(3 << LOG_LEVEL_SHIFT)

#define	LOG_LEVEL(op)	((unsigned)((op) & LOG_LEVEL_MASK))

/*
 * Log Op Codes
 * These are the events that can be logged.
 */

#define	LOG_START		 0 | LOG_CRITICAL	// always the first log entry.  Entered automatically, not by caller.
#define	LOG_IG_IPA_OPEN		 1 | LOG_CRITICAL
#define	LOG_IG_IPA_CLOSE	 2 | LOG_NORMAL
#define	LOG_IG_N2O_OPEN		 3 | LOG_CRITICAL
#define	LOG_IG_N2O_CLOSE	 4 | LOG_NORMAL
#define	LOG_SPARK_FIRST		 5 | LOG_CRITICAL
#define	LOG_SPARK_EACH		 6 | LOG_DETAIL
#define	LOG_SPARK_LAST		 7 | LOG_NORMAL
#define	IG_PRESSURE_GOOD_1	 8 | LOG_CRITICAL	// first time ig pressure is good
#define	IG_PRESSURE_GOOD	 9 | LOG_NORMAL		// second thru n'th time ig pressure is good
#define	IG_PRESSURE_CHANGE	10 | LOG_DETAIL		// any ig pressure change.  Param is 8 msb of pressure
#define	MAIN_N2O_CHANGE		11 | LOG_CRITICAL	// N2O valve commanded to move; param is 8 msb of servo commanded position
#define	MAIN_IPA_CHANGE		12 | LOG_CRITICAL
#define	LOG_TIME_ROLLOVER	99 | LOG_CRITICAL
