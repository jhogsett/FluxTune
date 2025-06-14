#include <Arduino.h>
#include "async_rtty.h"


AsyncRTTY::AsyncRTTY()
{
}    

bool AsyncRTTY::start_step_element(unsigned long time){
    async_element_done = false;
    async_element = 0;
    return false;
}

void AsyncRTTY::start_rtty(bool repeat){
    async_repeat = repeat;
    // async_element = 0;
    async_active = false;
    async_next_event = 0L;

    async_element_done = true;

    if(!start_step_element(millis()))
        return;
}    


unsigned long AsyncRTTY::compute_element_time(unsigned long time, bool stop_bit){
    return time + (stop_bit ? RTTY_TIME_BASIS2 : RTTY_TIME_BASIS);
}

int AsyncRTTY::step_element(unsigned long time){
    if(async_element_done){
        return STEP_ELEMENT_DONE;
    }
    
    if(time < async_next_event){
        return STEP_ELEMENT_EARLY;
    }

    switch(async_element){
        case 0:
            // start bit SPACE
            async_active = false;
            async_next_event = compute_element_time(time, false);
            async_element++;
            break;
            
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            async_active = (random(2) == 1 ? true : false);
            async_next_event = compute_element_time(time, false);
            async_element++;
            break;

        case 6:
            async_active = true;
            async_next_event = compute_element_time(time, true);
            async_element = 0;
    }

    return STEP_ELEMENT_ACTIVE;
}

int AsyncRTTY::step_rtty(unsigned long time){
    step_element(time);

    if(async_switched_on){
        if(async_active){
            return STEP_RTTY_LEAVE_ON;
        } else {
            async_switched_on = false;
            return STEP_RTTY_TURN_OFF;
        } 
    } else { // switched off
        if(!async_active){
            return STEP_RTTY_LEAVE_OFF;
        } else {
            async_switched_on = true;
            return STEP_RTTY_TURN_ON;
        }
    }
}


