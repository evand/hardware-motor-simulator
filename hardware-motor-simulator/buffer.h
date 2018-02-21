/*
 * Buffer for use in printing things
 */

#define	BUFFER_LEN		30	// size of buffer
#define	BUFFER_LEN_SHORT	20	// size of a line on the LCD

extern char buffer[];

extern void buffer_zip();
extern void buffer_zip_short();
extern void buffer_print_n_i(char col, int val);
extern void buffer_print_n_c(char col, unsigned char val);
