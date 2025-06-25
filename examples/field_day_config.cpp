/*
 * Field Day Station Configuration Example
 * 
 * This demonstrates how to configure FluxTune stations for ARRL Field Day
 * by overriding the default CQ message format.
 */

#include <Arduino.h>

// Override the default CQ format for Field Day operations
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "

// Now include the station headers - they will use the Field Day format
#include "sim_station.h"
#include "wave_gen_pool.h"
#include "signal_meter.h"

// Example setup for four Field Day CW stations
void setup_field_day_stations() {
    // These stations will now transmit "CQ FD CQ FD DE [CALL] [CALL] K"
    // instead of the default "CQ CQ DE [CALL] [CALL] K"
    
    static WaveGen wavegens[4];
    static bool statuses[4] = {false, false, false, false};
    static WaveGenPool pool(wavegens, statuses, 4);
    static SignalMeter meter;
    
    // Create four Field Day stations on different frequencies
    static SimStation fd_station1(&pool, &meter, 14050.0f, 20);  // 20m CW
    static SimStation fd_station2(&pool, &meter, 7025.0f, 18);   // 40m CW  
    static SimStation fd_station3(&pool, &meter, 3525.0f, 22);   // 80m CW
    static SimStation fd_station4(&pool, &meter, 21025.0f, 19);  // 15m CW
    
    // All stations will automatically use Field Day format!
}

/*
 * To use regular CQ format in the same build, you can:
 * 1. Include stations before defining CQ_MESSAGE_FORMAT (they use default)
 * 2. Or create a separate header file with the override
 * 3. Or use conditional compilation based on a build flag
 */

#ifdef FIELD_DAY_MODE
    // Field Day stations already configured above
#else
    // Regular contest stations would use default format
    void setup_regular_stations() {
        // These would use "CQ CQ DE [CALL] [CALL] K" format
    }
#endif
