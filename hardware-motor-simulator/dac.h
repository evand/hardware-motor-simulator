#define	DAC_IG		0	// pressure sensor simulator for ignitor
#define	DAC_MAIN	1	// pressure sensor simulator for main chamber

extern void dac_setup();
extern void dac_set(int dac, int val);
extern bool dac_ig_press_present();
