#include "MEAM_general.h"
#include "m_usb.h"
/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file

#define PRINTTIME(x) m_usb_tx_hex(x);m_usb_tx_char(10);m_usb_tx_char(13)

static int oldstate = 0;

void checkPB5(){
    int pinstate = bit_is_set(PINB,5);
    if(pinstate!=oldstate){
        PRINTTIME(TCNT3);
        PRINTTIME(pinstate);
    }
    oldstate = pinstate;
}

int main(void)
{
    m_usb_init();
    _clockdivide(0); //set the clock speed to 16Mhz
    set(DDRB,6); // set pin B6 to output
    //m_usb_init();

    clear(DDRB,5); //set the pin to input
    clear(PORTB,5); //turn off internal pullup

    //set timer with prescaler of 1024
    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30);

    set_led(OFF);			// turn on the on board LED
    m_usb_tx_string("hello");
    _delay_ms(1000);		// wait 1000 ms when at 16 MHz


    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        checkPB5();
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