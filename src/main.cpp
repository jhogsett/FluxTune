#include <Arduino.h>
#include <Wire.h>
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

#include <Encoder.h>
#include "encoder_handler.h"

#include "vfo.h"
#include "vfo_tuner.h"
#include "event_dispatcher.h"

#include "contrast.h"
#include "contrast_handler.h"

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

VFO vfoa("VFO A",   7000000, 100, 0);
VFO vfob("VFO B",  14300000, 500, 0);
VFO vfoc("VFO C", 146520000, 5000, 0);

VFO vfod("OUTPUT 1", 700, 1, 0);
VFO vfoe("OUTPUT 2", 1000, 1, 0);
VFO vfof("OUTPUT 3", 1, 1, 0);

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
#define APP_AUDIOOUT 2
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

}	

bool main_menu(){
	// bool buttons[] = {false, true, true, true};
	// return branch_prompt(FSTR("FluxTune"), NULL, NULL, NULL, NULL, buttons);
    return true;
}



EventDispatcher * set_application(int application){
	EventDispatcher *dispatcher;
	char *title;

	switch(application){
		case APP_SIMRADIO:
			dispatcher = &dispatcher1;
			current_dispatcher = APP_SIMRADIO;
			title = (FSTR("SimRadio"));
		break;

		case APP_AUDIOOUT:
		dispatcher = &dispatcher2;
			current_dispatcher = APP_AUDIOOUT;
			title = (FSTR("AudioOut"));
		break;

		case APP_SETTINGS:
			dispatcher = &dispatcher3;
			current_dispatcher = APP_SETTINGS;
			title = (FSTR("Settings"));
		break;
	}
		
	display.scroll_string(title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
	dispatcher->set_mode(&display, 0);

	return dispatcher;
}

void loop()
{
    display.scroll_string(FSTR("FLuXTuNE"), DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
    unsigned long time = millis();
    panel_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, DEFAULT_PANEL_LEDS_SHOW_TIME, DEFAULT_PANEL_LEDS_BLANK_TIME);

	set_application(APP_SIMRADIO);

	while(true){
        unsigned long time = millis();
        panel_leds.step(time);
		
		encoder_handlerA.step();
		encoder_handlerB.step();

		// check for changing dispatchers
		bool pressed = encoder_handlerB.pressed();
		bool long_pressed = encoder_handlerB.long_pressed();
		if(pressed || long_pressed){
			if(pressed){
				char *title;
				switch(current_dispatcher){
					case 1:
						dispatcher = &dispatcher2;
						current_dispatcher = 2;
						title = (FSTR("AudioOut"));
						break;

					case 2:
						dispatcher = &dispatcher3;
						current_dispatcher = 3;
						title = (FSTR("Settings"));
						break;
						
					case 3:
						dispatcher = &dispatcher1;
						current_dispatcher = 1;
						title = (FSTR("SimRadio"));
						break;
				}
					
				display.scroll_string(title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
				dispatcher->set_mode(&display, 0);

				// empty outstanding events
				encoder_handlerA.changed();
				encoder_handlerB.changed();
				encoder_handlerA.pressed();
				encoder_handlerA.long_pressed();
				encoder_handlerB.pressed();
				encoder_handlerB.long_pressed();
			}
		}

		if(encoder_handlerA.changed()){
			dispatcher->dispatch_event(&display, ID_ENCODER_TUNING, encoder_handlerA.diff(), 0);
			dispatcher->update_display(&display);
			dispatcher->update_realization();
		}

		if(encoder_handlerB.changed()){
			dispatcher->dispatch_event(&display, ID_ENCODER_MODES, encoder_handlerB.diff(), 0);
			dispatcher->update_display(&display);
			dispatcher->update_realization();
		}

		pressed = encoder_handlerA.pressed();
		long_pressed = encoder_handlerA.long_pressed();
		if(pressed || long_pressed){
			dispatcher->dispatch_event(&display, ID_ENCODER_TUNING, pressed, long_pressed);
		}
	}
}
