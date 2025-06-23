#ifndef __SEEDING_H__
#define __SEEDING_H__

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif
#include <random_seed.h>

// LSB (noisiest bit) of an analog read on an unused analog pin
// used to fill an int, one bit at a time, to create the random seed
// JH: former pin A1 was tied to a wire and not generating noise
#define RANDOM_SEED_PIN A7

extern RandomSeed<RANDOM_SEED_PIN> randomizer;

#endif
