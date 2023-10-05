/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file

int main(void)
{
    _clockdivide(0); //set the clock speed to 16Mhz
    set(DDRB,6); // set pin B6 to output
    //m_usb_init();

    clear(DDRB,5); //set the pin to input
    set(PORTB,5); //turn on internal pullup
    set_led(OFF);			// turn on the on board LED
    _delay_ms(1000);		// wait 1000 ms when at 16 MHz

    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        if(bit_is_set(PINB,5)){ //have to go to PINB register to read
        set_led(OFF);	// switch the led state
        clear(PORTB,6); //turn external led state
        } else {
            set_led(ON);
            set(PORTB,6);
        }
    }
    return 0;   /* never reached */
}