#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#include <Wire.h>
#include <MD_AD9833.h>
#include <Encoder.h>
#include <PololuLedStrip.h>
#endif

#include "displays.h"
#include "hardware.h"
#include "led_handler.h"
#include "leds.h"
#include "saved_data.h"
#include "seeding.h"
#include "utils.h"

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#ifndef PLATFORM_NATIVE
// These are already included in platform.h for native builds
#include <Encoder.h>
#include <PololuLedStrip.h>
#endif
#include "encoder_handler.h"

#include "vfo.h"
#include "vfo_tuner.h"
#include "event_dispatcher.h"

#include "contrast.h"
#include "contrast_handler.h"

#include "wavegen.h"
#include "wave_out.h"

#include "sim_station.h"
#include "sim_rtty.h"

#include "async_morse.h"

#include "realizer_pool.h"

#ifndef PLATFORM_NATIVE
// Already included in platform.h for native builds  
#include <PololuLedStrip.h>
#endif

// extend visual apparent range by modulating last led's brightness
// establish brightness baseline and ability to adjust overall brightness
// apply a damping function


// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 7
rgb_color colors[LED_COUNT] = 
{
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 15, 15, 0 }, 
  { 15, 15, 0 }, 
  { 15, 0, 0 } 
};

rgb_color empty[LED_COUNT] = 
{
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 } 
};


#define INTERVAL 100
unsigned long next_time = 0;
int value = 127;

void step_sm(unsigned long time)
{
	if(time < next_time)
		return;
	next_time = time + INTERVAL;

	int r = random(11) - 5;
	value += r;

	if(value > 255)
		value = 255;
	if(value < 0)
		value = 0;

	int sample = value * 2;
	int on_leds = (sample / 73) + 1;
	int remain = ((sample % 73) * 16) / 73;

	// Serial.println(remain);

//   remain = remain / 73
//   print(full_on)
//   print(remain)



//   int count = value / 8;
//   if(count > 7)
//     count = 7;

	// 

	rgb_color dbuffer[LED_COUNT];
	memcpy(dbuffer, colors, on_leds * sizeof(rgb_color));
	memcpy(dbuffer + on_leds, empty, (LED_COUNT - on_leds) * sizeof(rgb_color));

	// for the last copied LED, modify it according to the remain value
	dbuffer[on_leds-1].red = (dbuffer[on_leds-1].red * remain) / 16;
	dbuffer[on_leds-1].green = (dbuffer[on_leds-1].green * remain) / 16;
	dbuffer[on_leds-1].blue = (dbuffer[on_leds-1].blue * remain) / 16;

	// modify whole display per display contrast
	for(byte i = 0; i < LED_COUNT; i++){
		dbuffer[i].red = (dbuffer[i].red * option_contrast)	/ 1;
		dbuffer[i].green = (dbuffer[i].green * option_contrast)	/ 1;
		dbuffer[i].blue = (dbuffer[i].blue * option_contrast)	/ 1;
	}

	ledStrip.write(dbuffer, LED_COUNT);
}


#define CLKA 3
#define DTA 2
#define SWA 4

#define CLKB 6
#define DTB 5
#define SWB 7

#define PULSES_PER_DETENT 2

// Display handling
// show a display string for 700ms before beginning scrolling for ease of reading
#define DISPLAY_SHOW_TIME 800
// scroll the display every 90ms for ease of reading
#define DISPLAY_SCROLL_TIME 70
// scroll flipped options every 100ms
#define OPTION_FLIP_SCROLL_TIME 100

EncoderHandler encoder_handlerA(0, CLKA, DTA, SWA, PULSES_PER_DETENT);
EncoderHandler encoder_handlerB(1, CLKB, DTB, SWB, PULSES_PER_DETENT);


// Pins for SPI comm with the AD9833 IC
const byte PIN_DATA = 11;  ///< SPI Data pin number
const byte PIN_CLK = 13;  	///< SPI Clock pin number
const byte PIN_FSYNC1 = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)
const byte PIN_FSYNC2 = 14;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const byte PIN_FSYNC3 = 15;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const byte PIN_FSYNC4 = 16;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833 AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833 AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833 AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
MD_AD9833 AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

WaveGen wavegen1(&AD1);
WaveGen wavegen2(&AD2);
WaveGen wavegen3(&AD3);
WaveGen wavegen4(&AD4);

Realizer *realizers[4] = {&wavegen1, &wavegen2, &wavegen3, &wavegen4};
bool realizer_stats[4] = {false, false, false, false};
RealizerPool realizer_pool(realizers, realizer_stats, 4);

SimStation simstation1(&realizer_pool);
SimStation simstation2(&realizer_pool);
SimStation simstation3(&realizer_pool);
// SimStation simstation4(&realizer_pool);

WaveOut waveout1(&realizer_pool);
WaveOut waveout2(&realizer_pool);
WaveOut waveout3(&realizer_pool);
WaveOut waveout4(&realizer_pool);

// Realization *simstations[] = {&simstation1, &simstation2, &simstation3, &simstation4}; 

// VFO vfoa("VFO A",   7000000.0, 10, &realization_pool);
// VFO vfob("VFO B",  14000000.0, 10, &realization_pool);
// VFO vfoc("VFO C", 146520000.0, 5000, &realization_pool);

// SimRTTY simstation1(&wavegen1, 7005000.0);
// SimRTTY simstation2(&wavegen2, 7006000.0);
// SimRTTY simstation3(&wavegen3, 7007000.0);
SimRTTY simstation4(&realizer_pool);

Realization *realizations[4] = {&simstation1, &simstation2, &simstation3, &simstation4}; 
bool realization_stats[4] = {false, false, false, false};
RealizationPool realization_pool(realizations, realization_stats, 4);

VFO vfoa("VFO A",   7000000.0, 10, &realization_pool);
VFO vfob("VFO B",  14000000.0, 10, &realization_pool);
VFO vfoc("VFO C", 146520000.0, 5000, &realization_pool);

Realization *waveouts[] = {&waveout1};

VFO vfod("CHAN 1", 1.0, 1L, &realization_pool);
VFO vfoe("CHAN 2", 100.0, 10L, &realization_pool);
VFO vfof("CHAN 3", 1000000.0, 100L, &realization_pool);

Contrast contrast("Contrast");

VFO_Tuner tunera(&vfoa);
VFO_Tuner tunerb(&vfob);
VFO_Tuner tunerc(&vfoc);

VFO_Tuner tunerd(&vfod);
VFO_Tuner tunere(&vfoe);
VFO_Tuner tunerf(&vfof);

ContrastHandler contrast_handler(&contrast);

ModeHandler *handlers1[3] = {&tunera, &tunerb, &tunerc};
ModeHandler *handlers2[3] = {&tunerd, &tunere, &tunerf};
ModeHandler *handlers3[1] = {&contrast_handler};

EventDispatcher dispatcher1(handlers1, 3);
EventDispatcher dispatcher2(handlers2, 3);
EventDispatcher dispatcher3(handlers3, 1);

EventDispatcher * dispatcher = &dispatcher1;
int current_dispatcher = 1;


#define APP_SIMRADIO 1
#define APP_WAVEGEN 2
#define APP_SETTINGS 3

void setup_display(){
	Wire.begin();

	const byte display_brightnesses[] = {(unsigned char)option_contrast, (unsigned char)option_contrast};
	display.init(display_brightnesses);
	/* the duplicated displays do not need reinitialization
	disp1.init(brightness+0);
	disp2.init(brightness+1);
	disp3.init(brightness+2); */
	display.clear();
}

void setup_leds(){
	for(byte i = FIRST_LED; i <= LAST_LED; i++){
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	unsigned long time = millis();
	panel_leds.begin(time, LEDHandler::STYLE_RANDOM, DEFAULT_PANEL_LEDS_SHOW_TIME, DEFAULT_PANEL_LEDS_BLANK_TIME);
	// button_leds.begin(time, LEDHandler::STYLE_BLANKING, DEFAULT_BUTTON_LEDS_SHOW_TIME, DEFAULT_BUTTON_LEDS_BLANK_TIME);
	// all_leds.begin(time, LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING | LEDHandler::STYLE_MIRROR, DEFAULT_ALL_LEDS_SHOW_TIME, DEFAULT_ALL_LEDS_BLANK_TIME);
}

void setup_buttons(){
	// for(byte i = 0; i < NUM_BUTTONS; i++){
	// 	pinMode(i + FIRST_BUTTON, 0x03); // INPUT_PULLDOWN ?!
	// 	button_states[i] = false;
	// }
}

void setup(){
	Serial.begin(115200);
	randomizer.randomize();

	load_save_data();

	setup_leds();
	setup_display();
	// setup_buttons();


    // // if all three buttons are pressed, enable auto play
    // if((digitalRead(GREEN_BUTTON) == HIGH) && (digitalRead(AMBER_BUTTON) == HIGH) && (digitalRead(RED_BUTTON) == HIGH)){
    //     auto_play_enabled = true;
    // }

	// attachInterrupt(digitalPinToInterrupt(ANY_BUTTON), button_pressed_i, RISING);
	// button_states[ANY_COLOR_ID] = false;

    // send_morse("Infinity Game");

	// if(option_sound)
	// 	beep();

	// if(option_vibrate)
	// 	vibrate();

	// dispatcher->set_mode(&display, 0);

	AD1.begin();
	AD1.setFrequency((MD_AD9833::channel_t)0, 0.1);
	AD1.setFrequency((MD_AD9833::channel_t)1, 0.1);
	AD1.setMode(MD_AD9833::MODE_SINE);

	AD2.begin();
	AD2.setFrequency((MD_AD9833::channel_t)0, 0.1);
	AD2.setFrequency((MD_AD9833::channel_t)1, 0.1);
	AD2.setMode(MD_AD9833::MODE_SINE);

	AD3.begin();
	AD3.setFrequency((MD_AD9833::channel_t)0, 0.1);
	AD3.setFrequency((MD_AD9833::channel_t)1, 0.1);
	AD3.setMode(MD_AD9833::MODE_SINE);

	AD4.begin();
	AD4.setFrequency((MD_AD9833::channel_t)0, 0.1);
	AD4.setFrequency((MD_AD9833::channel_t)1, 0.1);
	AD4.setMode(MD_AD9833::MODE_SINE);
}	

bool main_menu(){
    return true;
}



EventDispatcher * set_application(int application, HT16K33Disp *display){
	EventDispatcher *dispatcher;
	char *title;

	switch(application){
		case APP_SIMRADIO:
			dispatcher = &dispatcher1;
			current_dispatcher = APP_SIMRADIO;
			title = (FSTR("SimRadio"));
		break;

		case APP_WAVEGEN:
		dispatcher = &dispatcher2;
			current_dispatcher = APP_WAVEGEN;
			title = (FSTR("Wave Gen"));
		break;

		case APP_SETTINGS:
			dispatcher = &dispatcher3;
			current_dispatcher = APP_SETTINGS;
			title = (FSTR("Settings"));
		break;
	}
		
	display->scroll_string(title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
	dispatcher->set_mode(display, 0);

	// // empty outstanding events
	// encoder_handlerA.changed();
	// encoder_handlerB.changed();
	// encoder_handlerA.pressed();
	// encoder_handlerA.long_pressed();
	// encoder_handlerB.pressed();
	// encoder_handlerB.long_pressed();



	// display.scroll_string(title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
	// dispatcher->set_mode(&display, 0);

	return dispatcher;
}

void purge_events(){
	while(encoder_handlerA.changed());
	while(encoder_handlerB.changed());

	while(encoder_handlerA.pressed());
	while(encoder_handlerA.long_pressed());

	while(encoder_handlerB.pressed());
	while(encoder_handlerB.long_pressed());
}

#define PURGE_TIME 1000

void loop()
{
    display.scroll_string(FSTR("FLuXTuNE"), DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
    unsigned long time = millis();
    panel_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, DEFAULT_PANEL_LEDS_SHOW_TIME, DEFAULT_PANEL_LEDS_BLANK_TIME);
	simstation1.begin(time + random(1000), 7002000.0, "CQ CQ DE N6CCM N6CCM K    ", 11);
	simstation2.begin(time + random(1000), 7002500.0, "CQ CQ DE N6CCM N6CCM K    ", 13);
	simstation3.begin(time + random(1000), 7003000.0, "CQ CQ DE N6CCM N6CCM K    ", 20);

	simstation4.begin(time + random(1000), 7010000.0);
	// simstation4.begin(time + random(1000), 7003500.0, "CQ CQ DE N16CCM N6CCM K    ", 24);

	set_application(APP_SIMRADIO, &display);

	// AsyncMorse morse1;
	// AsyncMorse morse2;
	// // start_morse("CQ CQ CQ DE N6CCM N6CCM K              ", 20, true);
	// morse1.start_morse("CQ CQ DE N6CCM N6CCM K              ", 5, true);
	// morse2.start_morse("CQ CQ DE N6CCM N6CCM K              ", 13, true);

	// AD1.setFrequency((MD_AD9833::channel_t)0, 0.1);
	// AD1.setFrequency((MD_AD9833::channel_t)1, 0.1);

	// AD2.setFrequency((MD_AD9833::channel_t)0, 0.1);
	// AD2.setFrequency((MD_AD9833::channel_t)1, 0.1);

	// bool active = false;
	// bool freq = false;
	// bool last_active = true;
	while(true){
        unsigned long time = millis();

		step_sm(time);

		// switch(morse1.step_morse(time)){
		// 	case STEP_MORSE_TURN_ON:
		// 		AD1.setActiveFrequency((MD_AD9833::channel_t)0);
		// 		// AD1.setFrequency((MD_AD9833::channel_t)0, 900.0);
		// 		break;

		// 	case STEP_MORSE_TURN_OFF:
		// 		AD1.setActiveFrequency((MD_AD9833::channel_t)1);
		// 		// AD1.setFrequency((MD_AD9833::channel_t)0, 0.0);
		// 		break;
		// }

		// switch(morse2.step_morse(time)){
		// 	case STEP_MORSE_TURN_ON:
		// 		AD2.setActiveFrequency((MD_AD9833::channel_t)0);
		// 		break;

		// 	case STEP_MORSE_TURN_OFF:
		// 		AD2.setActiveFrequency((MD_AD9833::channel_t)1);
		// 		break;
		// }

        panel_leds.step(time);

		realization_pool.step(time);

		// simstation1.step(time);
		// simstation2.step(time);
		// simstation3.step(time);
		// simstation4.step(time);

		encoder_handlerA.step();
		encoder_handlerB.step();

		// check for changing dispatchers
		bool pressed = encoder_handlerB.pressed();
		bool long_pressed = encoder_handlerB.long_pressed();
		if(pressed || long_pressed){
			if(pressed){
				// char *title;
				switch(current_dispatcher){
					case 1:
						// 
						dispatcher = set_application(APP_WAVEGEN, &display); // &dispatcher2;
						// current_dispatcher = 2;
						// title = (FSTR("AudioOut"));
						break;
						
						case 2:
						// 
						dispatcher = set_application(APP_SETTINGS, &display); // &dispatcher3;
						// current_dispatcher = 3;
						// title = (FSTR("Settings"));
						break;
						
						case 3:
						// 
						dispatcher = set_application(APP_SIMRADIO, &display); // &dispatcher1;
						// current_dispatcher = 1;
						// title = (FSTR("SimRadio"));
						break;
				}

				purge_events();
					
				// display.scroll_string(title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
				// dispatcher->set_mode(&display, 0);

				// // empty outstanding events
				// encoder_handlerA.changed();
				// encoder_handlerB.changed();
				// encoder_handlerA.pressed();
				// encoder_handlerA.long_pressed();
				// encoder_handlerB.pressed();
				// encoder_handlerB.long_pressed();
			}
		}

		if(encoder_handlerA.changed()){
			dispatcher->dispatch_event(&display, ID_ENCODER_TUNING, encoder_handlerA.diff(), 0);
			dispatcher->update_display(&display);
			dispatcher->update_realization();
		}

		if(encoder_handlerB.changed()){
			dispatcher->dispatch_event(&display, ID_ENCODER_MODES, encoder_handlerB.diff(), 0);

			purge_events();

			dispatcher->update_display(&display);
			dispatcher->update_realization();

			// after mode change
			unsigned long endtime = millis() + PURGE_TIME;
			while(millis() < endtime)
				purge_events();
		}

		pressed = encoder_handlerA.pressed();
		long_pressed = encoder_handlerA.long_pressed();
		if(pressed || long_pressed){
			dispatcher->dispatch_event(&display, ID_ENCODER_TUNING, pressed, long_pressed);
		}
	}
}
