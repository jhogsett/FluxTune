#include <Arduino.h>
#include <Wire.h>

#include <MD_AD9833.h>
// #include <SPI.h>

// #include "buttons.h"
#include "displays.h"
#include "hardware.h"
#include "idle_mode.h"
#include "led_handler.h"
#include "leds.h"
// #include "motor.h"
#include "options_mode.h"
#include "saved_data.h"
// #include "prompts.h"
#include "seeding.h"
// #include "slot_game.h"
// #include "speaker.h"
// #include "time_game.h"
#include "utils.h"
// #include "word_game.h"

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include "encoder_handler.h"

#include "vfo.h"
#include "vfo_tuner.h"
#include "event_dispatcher.h"

#include "contrast.h"
#include "contrast_handler.h"

#include "wavegen.h"
#include "wave_out.h"

#include "sim_signal.h"

#include "async_morse.h"

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
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)
// const uint8_t PIN_FSYNC2 = 9;  ///< SPI Load pin number (FSYNC in AD9833 usage)
// const uint8_t PIN_FSYNC3 = 8;  ///< SPI Load pin number (FSYNC in AD9833 usage)
// const uint8_t PIN_FSYNC4 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC); // Arbitrary SPI pins
// MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
// MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
// MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

WaveGen wavegen1(&AD1);
WaveOut waveout1(&wavegen1);
SimSignal simsignal1(&wavegen1);

VFO vfoa("VFO A",   7000000.0, 50, &simsignal1);
VFO vfob("VFO B",  14300000.0, 500, &simsignal1);
VFO vfoc("VFO C", 146520000.0, 5000, &simsignal1);

VFO vfod("CHAN 1", 1.0, 1L, &waveout1);
VFO vfoe("CHAN 2", 100.0, 10L, &waveout1);
VFO vfof("CHAN 3", 1000000.0, 100L, &waveout1);

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


#define SILENT_FREQ 1000000.0


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
	AD1.setFrequency((MD_AD9833::channel_t)0, 1000000.0);
	AD1.setFrequency((MD_AD9833::channel_t)1, 0.1);
	AD1.setMode(MD_AD9833::MODE_SINE);
}	

bool main_menu(){
	// bool buttons[] = {false, true, true, true};
	// return branch_prompt(FSTR("FluxTune"), NULL, NULL, NULL, NULL, buttons);
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

	simsignal1.begin(millis(), 45);

	set_application(APP_SIMRADIO, &display);

	AsyncMorse morse;
	// start_morse("CQ CQ CQ DE N6CCM N6CCM K              ", 20, true);
	morse.start_morse("CQ CQ DE N6CCM N6CCM K              ", 20, true);

	AD1.setFrequency((MD_AD9833::channel_t)0, 700.0);
	AD1.setFrequency((MD_AD9833::channel_t)1, 0.1);

	// bool active = false;
	// bool freq = false;
	// bool last_active = true;
	while(true){
        unsigned long time = millis();
		switch(morse.step_morse(time)){
			case STEP_MORSE_TURN_ON:
				AD1.setActiveFrequency((MD_AD9833::channel_t)0);
				break;

			case STEP_MORSE_TURN_OFF:
				AD1.setActiveFrequency((MD_AD9833::channel_t)1);
				break;

			case STEP_MORSE_LEAVE_ON:
				break;

			case STEP_MORSE_LEAVE_OFF:
				break;
		}

		// Serial.print(active ? "A" : "n");

		// need to know if not sending, no need to set silent freq

		// if(active){
		// 	if(!freq){
		// 		AD1.setActiveFrequency((MD_AD9833::channel_t)0);
		// 		freq = true;
		// 	}
		// } else {
		// 	if(freq){
		// 		AD1.setActiveFrequency((MD_AD9833::channel_t)1);
		// 	freq = false;
		// 	}
		// }

        panel_leds.step(time);
		simsignal1.step(time);
		
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
			//@@@
			// dispatcher->update_realization();
		}

		if(encoder_handlerB.changed()){
			dispatcher->dispatch_event(&display, ID_ENCODER_MODES, encoder_handlerB.diff(), 0);

			purge_events();

			dispatcher->update_display(&display);
			// dispatcher->update_realization(&wavegen1);

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
