#define	DAC_MAIN	0	// pressure sensor simulator for main chamber
#define	DAC_IG		1	// pressure sensor simulator for ignitor

extern void dac_setup();
extern void dac_set(int dac, int val);
extern bool dac_ig_press_present();
