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
#include "play_data.h"
// #include "prompts.h"
#include "seeding.h"
// #include "slot_game.h"
// #include "speaker.h"
// #include "time_game.h"
#include "utils.h"
// #include "word_game.h"

#include <Encoder.h>
#include "encoder_handler.h"

#define CLKA 2
#define DTA 3
#define SWA 4

#define CLKB 5
#define DTB 6
#define SWB 7

#define PULSES_PER_DETENT 2

EncoderHandler encoder_handlerA(0, CLKA, DTA, SWA, PULSES_PER_DETENT);
EncoderHandler encoder_handlerB(1, CLKB, DTB, SWB, PULSES_PER_DETENT);


void setup_display(){
	Wire.begin();
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

	setup_leds();
	setup_display();
	// setup_buttons();

	load_save_data();

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

}

bool main_menu(){
	// bool buttons[] = {false, true, true, true};
	// return branch_prompt(FSTR("FluxTune"), NULL, NULL, NULL, NULL, buttons);
    return true;
}

void loop()
{
    display.show_string(FSTR("FLuXTuNE"));
    unsigned long time = millis();
    panel_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, DEFAULT_PANEL_LEDS_SHOW_TIME, DEFAULT_PANEL_LEDS_BLANK_TIME);

    while(true){
        unsigned long time = millis();
        panel_leds.step(time);
		
		encoder_handlerA.step();
		encoder_handlerB.step();
	}
}
