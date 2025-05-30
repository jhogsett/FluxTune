#ifndef __SAVED_DATA_H
#define __SAVED_DATA_H

#include <Arduino.h>
#include "idle_mode.h"
#include "morse.h"
#include "chime.h"

// when adding new persisted play data, search for ##DATA

// ##DATA Increment the save data version to force upgraded devices to auto-reset after programming
// Current save data version
// On start-up if this differs from the EEPROM value, the data is reset to defaults
#define SAVE_DATA_VERSION 1

#define DEFAULT_CONTRAST 2

// // All money originates in the bank, this is in money basis units
// #define DEFAULT_OUTSTANDING (DEFAULT_GANG + DEFAULT_HOUSE + DEFAULT_PURSE)
// #define DEFAULT_BANK (2000000000L - DEFAULT_OUTSTANDING)

// // All bets are paid from the player purse
// // all wins received are kept in the player purse
// // the player purse draws money from the gang when needed
// #define DEFAULT_PURSE  (1000L / MONEY_BASIS)

// // All bets are paid to the house
// // All payouts are from the house
// // The house draws money from the bank when needed
// #define DEFAULT_HOUSE (1000000L / MONEY_BASIS)

// // The gang robs money from the bank
// // Player cash comes from the gang
// #define DEFAULT_GANG (100000L / MONEY_BASIS)

// the longest possible count of milliseconds
#define DEFAULT_TIME ((unsigned long)-1)

// default idle mode
#define DEFAULT_IDLE_MODE IDLE_MODE_SLEEP

// default milliseconds until device goes into idle mode
#define DEFAULT_IDLE_TIME (5L * 60L * 1000L)

// #define DEFAULT_VIG 0L

// #define DEFAULT_VIB_STR false

// #define DEFAULT_CLOCK_CHIME CHIME_NONE

// #define DEFAULT_WPM DEFAULT_MORSE_WPM

// ##DATA add new defaults on play data reset above here


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

// Additional slots for best times (future use)
// extern unsigned long best_time1;
// extern unsigned long best_time2;
// extern unsigned long best_time3;

// Current house
// extern long house;

// Current gang
// extern long gang;

// Idle Time in milliseconds
// extern unsigned long option_idle_time;

// Current vig
// extern long vig;

// extern byte option_clock_chime;

// extern byte option_wpm;

// extern bool auto_play_enabled;

// ##DATA Add 'extern's for new persisted play data veriables here


// Saved data structure version 1
struct SavedData{
	byte version;

	int option_contrast;

	// bool option_sound;
	// bool option_vibrate;

	// unsigned long purse;
	// unsigned long best_time;

	// bool option_clock_24h;
	// byte option_idle_mode;

	// unsigned long bank;
	// unsigned long best_time1;
	// unsigned long best_time2;
	// unsigned long best_time3;
	// long house;
	// long gang;
	// unsigned long option_idle_time;
    // long vig;

	// bool option_vib_str;

	// byte option_clock_chime;

	// byte option_wpm;

	// bool auto_play_enabled;

	// ##DATA Add new persisted data types above here
};

extern void load_save_data();
extern void save_data();
extern bool reset_options();
extern void reset_device();

#endif
