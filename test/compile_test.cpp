#include "../native/mock_arduino.h"
#include "wave_gen_pool.h"
#include "realization.h"
#include "sim_station.h"
#include "sim_transmitter.h"

int main() {
    // Simple test to verify headers compile correctly
    WaveGenPool* pool = nullptr;
    Realization* realization = new Realization(pool);
    delete realization;
    return 0;
}
