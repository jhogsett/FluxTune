#include "../include/basic_types.h"
#ifdef NATIVE_BUILD
#include "../native/platform.h"
#include <iostream>
#else
#include <EEPROM.h>
#endif
#include "../include/saved_data.h"

int option_contrast = DEFAULT_CONTRAST;
int option_bfo_offset = DEFAULT_BFO_OFFSET;
int option_flashlight = DEFAULT_FLASHLIGHT;

void load_save_data(){
	SavedData saved_data;
	EEPROM.get(0, saved_data);

	if(saved_data.version != SAVE_DATA_VERSION){
		reset_options();
		return;
	}
	option_contrast = saved_data.option_contrast;
	option_bfo_offset = saved_data.option_bfo_offset;
	option_flashlight = saved_data.option_flashlight;
	// option_clock_24h = saved_data.option_clock_24h;
	// option_idle_mode = saved_data.option_idle_mode;
	// // bank = saved_data.bank;
	// best_time1 = saved_data.best_time1;
	// best_time2 = saved_data.best_time2;
	// best_time3 = saved_data.best_time3;
	// // house = saved_data.house;
	// // gang = saved_data.gang;
	// option_idle_time = saved_data.option_idle_time;
    // // vig = saved_data.vig;
    // option_vib_str = saved_data.option_vib_str;
    // option_clock_chime = saved_data.option_clock_chime;
    // option_wpm = saved_data.option_wpm;
    // auto_play_enabled = saved_data.auto_play_enabled;

	// ##DATA Load new persisted play data variables into memory here
}

void save_data(){	SavedData saved_data;	saved_data.version = SAVE_DATA_VERSION;
	saved_data.option_contrast = option_contrast;
	saved_data.option_bfo_offset = option_bfo_offset;
	saved_data.option_flashlight = option_flashlight;

	EEPROM.put(0, saved_data);
}

typedef void (*VoidFunc)(void);

void reset_device(){
#ifdef NATIVE_BUILD
	// For native builds, just print a message instead of crashing
	std::cout << "[MOCK] reset_device() called - would restart Arduino" << std::endl;
#else
	VoidFunc p = NULL;
	p();
#endif
}

bool reset_options(){
	option_contrast = DEFAULT_CONTRAST;
	option_bfo_offset = DEFAULT_BFO_OFFSET;

	save_data();

	reset_device();
	return false;
}
