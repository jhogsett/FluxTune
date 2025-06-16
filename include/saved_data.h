#ifndef __SAVED_DATA_H
#define __SAVED_DATA_H

#include "basic_types.h"
#include "chime.h"

// when adding new persisted play data, search for ##DATA

// ##DATA Increment the save data version to force upgraded devices to auto-reset after programming
// Current save data version
// On start-up if this differs from the EEPROM value, the data is reset to defaults
#define SAVE_DATA_VERSION 1

#define DEFAULT_CONTRAST 2

// the longest possible count of milliseconds
#define DEFAULT_TIME ((unsigned long)-1)

// Display time for interstitial displays during games
#define ROUND_DELAY 750

extern byte save_data_version;

extern int option_contrast;

// // Whether to play sounds
// extern bool option_sound;

// // Whether to use vibration feedback
// extern bool option_vibrate;

// // vibration strength true=hi false=lo
// extern bool option_vib_str;

// Player's Cash
// extern long purse;

// // Player's best time in The TimeGame
// extern unsigned long best_time;

// // Whether to display 12 or 24 hour time
// extern bool option_clock_24h;

// // mode when mode none, sleep, clock
// extern byte option_idle_mode;

// Current bank
// extern unsigned long bank;

// Saved data structure version 1
struct SavedData{
	byte version;
	int option_contrast;
};

extern void load_save_data();
extern void save_data();
extern bool reset_options();
extern void reset_device();

#endif
