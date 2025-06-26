/*
 * Resource Contention Test for FluxTune
 * 
 * This configuration tests the behavior of 5 CW stations competing for 4 wave generators.
 * 
 * CONFIGURATION: CONFIG_FIVE_CW_RESOURCE_TEST
 * 
 * Expected behavior:
 * 1. Only 4 stations should be audible at any given time
 * 2. The 5th station should be properly dormant (no phantom signals)
 * 3. Stations should gracefully cycle in and out as resources become available
 * 4. No crashes or undefined behavior should occur
 * 5. Signal meter should only activate for stations with allocated wave generators
 * 6. Panel lock LED should only light for properly allocated stations
 * 
 * Test procedure:
 * 1. Enable CONFIG_FIVE_CW_RESOURCE_TEST in station_config.h
 * 2. Build and upload to device
 * 3. Use SimRadio application to tune across 40m band (7.001 - 7.005 MHz)
 * 4. Count simultaneously active stations - should never exceed 4
 * 5. Watch signal meter LED array for each frequency
 * 6. Check white panel lock LED activation
 * 7. Monitor over 10+ minutes - dormant station should eventually rotate in
 * 
 * Station configuration:
 * - Station 1: 7.001 MHz, 15 WPM, slight fist variation (10)
 * - Station 2: 7.002 MHz, 18 WPM, moderate fist variation (20)  
 * - Station 3: 7.003 MHz, 12 WPM, noticeable fist variation (30)
 * - Station 4: 7.004 MHz, 22 WPM, slight fist variation (15)
 * - Station 5: 7.005 MHz, 25 WPM, moderate fist variation (25)
 * 
 * Resource allocation:
 * - Wave generator pool: 4 generators total (hardware limit)
 * - Realization pool: 5 stations competing for resources
 * - Each station releases its wave generator during 4-second wait periods
 * - Stations retry every 1 second when no resources available
 * 
 * Signs of proper operation:
 * ✓ 4 stations audible at any given time
 * ✓ 1 station dormant (silent) at any given time  
 * ✓ Signal meter only activates for audible stations
 * ✓ Panel lock LED only activates for audible stations
 * ✓ Over time, different combinations of 4 stations are active
 * ✓ No crashes or resets
 * 
 * Signs of problems:
 * ✗ More than 4 stations audible simultaneously
 * ✗ Phantom signal meter activity for dormant stations
 * ✗ Panel lock LED lighting for inaudible frequencies
 * ✗ All 5 stations silent (resource deadlock)
 * ✗ System crashes or resets
 * ✗ Same station always dormant (unfair resource allocation)
 * 
 * Current status: READY FOR TESTING
 * RAM usage: 85.9% (1759/2048 bytes) - within acceptable limits
 */
