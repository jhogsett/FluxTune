#include "station_manager.h"
#include "sim_numbers.h" // Example concrete station type
#include "sim_station.h"
#include "sim_rtty.h" 
#include "sim_pager.h"

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif

StationManager::StationManager(SimTransmitter* station_ptrs[MAX_STATIONS]) {
    for (int i = 0; i < MAX_STATIONS; ++i) {
        stations[i] = station_ptrs[i];
        stations[i]->setActive(false); // Set all stations dormant/inactive
    }
    for (int i = 0; i < MAX_AD9833; ++i) {
        ad9833_assignment[i] = -1;
    }
}

void StationManager::updateStations(uint32_t vfo_freq) {
    // Define proximity range for station activation (±50 kHz seems reasonable for HF)
    const uint32_t ACTIVATION_RANGE = 50000; // 50 kHz in Hz
    
    // Get current station frequencies (these would ideally be stored in the StationManager)
    // For now, hardcode the current frequencies - TODO: make this data-driven
    uint32_t station_frequencies[MAX_STATIONS] = {
        7002000,  // cw_station1: 7.002 MHz
        7002700,  // numbers_station1: 7.0027 MHz  
        7004100,  // rtty_station1: 7.0041 MHz
        7006000,  // pager_station1: 7.006 MHz
        0,        // cw_station2: not assigned yet
        0         // numbers_station2: not assigned yet
    };
    
    // Calculate which stations should be active based on VFO proximity
    bool should_be_active[MAX_STATIONS];
    int active_count = 0;
    
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (station_frequencies[i] == 0) {
            should_be_active[i] = false; // Unassigned stations stay dormant
        } else {
            uint32_t freq_diff = (vfo_freq > station_frequencies[i]) 
                ? (vfo_freq - station_frequencies[i])
                : (station_frequencies[i] - vfo_freq);
            should_be_active[i] = (freq_diff <= ACTIVATION_RANGE);
            if (should_be_active[i]) active_count++;
        }
    }
    
    // If too many stations would be active, prioritize by proximity
    if (active_count > MAX_AD9833) {
        // TODO: Implement priority-based selection
        // For now, just activate first 4 that are in range
        int activated = 0;
        for (int i = 0; i < MAX_STATIONS && activated < MAX_AD9833; i++) {
            if (should_be_active[i]) {
                activated++;
            } else {
                should_be_active[i] = false;
            }
        }
    }
    
    // Apply the activation/deactivation decisions
    for (int i = 0; i < MAX_STATIONS; i++) {
        bool currently_active = stations[i]->isActive();
        
        if (should_be_active[i] && !currently_active) {
            // Activate station
            activateStation(i, station_frequencies[i]);
        } else if (!should_be_active[i] && currently_active) {
            // Deactivate station  
            deactivateStation(i);
        }
    }
}

void StationManager::allocateAD9833() {
    // TODO: Assign AD9833 channels to the most relevant stations
}

void StationManager::recycleDormantStations(uint32_t vfo_freq) {
    // TODO: Reuse dormant stations for new frequencies as needed
}

SimTransmitter* StationManager::getStation(int idx) {
    if (idx >= 0 && idx < MAX_STATIONS) return stations[idx];
    return nullptr;
}

int StationManager::getActiveStationCount() const {
    int count = 0;
    for (int i = 0; i < MAX_STATIONS; ++i) {
        if (stations[i]->isActive()) ++count;
    }
    return count;
}

void StationManager::activateStation(int idx, uint32_t freq) {
    if (idx < 0 || idx >= MAX_STATIONS) return;
    
    // Get current time for station initialization
    unsigned long time = millis();
    
    // Initialize station based on its index/type
    // This is a temporary hardcoded approach - TODO: make this data-driven
    switch(idx) {
        case 0: // cw_station1
            // Cast to SimStation and initialize with CW parameters
            ((SimStation*)stations[idx])->begin(time + random(1000), freq / 1000000.0f, "CQ CQ DE N6CCM N6CCM K    ", 11);
            break;
        case 1: // numbers_station1
            // Cast to SimNumbers and initialize
            ((SimNumbers*)stations[idx])->begin(time + random(1000), freq / 1000000.0f, 18);
            break;
        case 2: // rtty_station1
            // Cast to SimRTTY and initialize
            ((SimRTTY*)stations[idx])->begin(time + random(1000), freq / 1000000.0f);
            break;
        case 3: // pager_station1
            // Cast to SimPager and initialize
            ((SimPager*)stations[idx])->begin(time + random(1000), freq / 1000000.0f);
            break;
        case 4: // cw_station2
            // Second CW station with different message
            ((SimStation*)stations[idx])->begin(time + random(1000), freq / 1000000.0f, "CQ CQ DE W1AW W1AW K    ", 13);
            break;
        case 5: // numbers_station2
            // Second numbers station with different WPM
            ((SimNumbers*)stations[idx])->begin(time + random(1000), freq / 1000000.0f, 20);
            break;
    }
    
    stations[idx]->setActive(true);
}

void StationManager::deactivateStation(int idx) {
    if (idx < 0 || idx >= MAX_STATIONS) return;
    
    // Call end() to properly release AD9833 generators
    stations[idx]->end();
    stations[idx]->setActive(false);
}

int StationManager::findDormantStation() {
    for (int i = 0; i < MAX_STATIONS; ++i) {
        if (!stations[i]->isActive()) return i;
    }
    return -1;
}
