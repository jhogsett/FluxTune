#include "clock.h"
#include "idle_mode.h"
#include "leds.h"
#include "saved_data.h"
// #include "prompts.h"
#include "sleep_mode.h"

bool idle_mode(){
	// panel_leds.deactivate_leds(true);

	// switch(option_idle_mode){
	// 	case IDLE_MODE_NONE:
	// 		break;
	// 	case IDLE_MODE_SLEEP:
	// 		sleep_mode();
	// 		break;
	// 	case IDLE_MODE_CLOCK:
	// 		clock_prompt(0, 0, 12, false);
	// 		break;
	// }

	return false;
}
