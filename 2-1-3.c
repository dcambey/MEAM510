#include "MEAM_general.h"
#include "m_usb.h"
/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#define PRINTTIME(x) m_usb_tx_uint(x);m_usb_tx_char(10);m_usb_tx_char(13)

int oldVal = 0;
int time_el = 0;
int counts_el = 0;
int button_down = 0;

void dolights(){
    if(bit_is_set(PINC,7)){ //have to go to PINB register to read
            clear(PORTB,6); //turn external led state
    } if(bit_is_clear(PINC,7)) {
            set(PORTB,6);
    }
}

void calcTime(){
    while(!bit_is_set(TIFR3,ICF3)) dolights();  //if input capture happened
        set(TIFR3,ICF3);
        if(button_down == 0){
            button_down = 1;
        } else {
            button_down = 0;
        }
        if(button_down == 1){ //if it just went low (button pressed down)   
        counts_el = ICR3 - oldVal; //twos complement works, how many counts passed
        
        PRINTTIME(counts_el);
        // PRINTTIME(counts_el*1024*1000/(16*10^6)); //print time since last depressed
        oldVal = ICR3; //set currrent input capture value as the old value
        }
    }
   

int main(void)
{
    m_usb_init();
    _clockdivide(0); //set the clock speed to 16Mhz
    clear(DDRC,7); // set pin C7 to input

    set(DDRB,6); //set pin B6 to output

    //set timer with prescaler of 1024
    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30);

    //set up input capture
    set(TCCR3B,ICES3); //store on rising edge

    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        calcTime();
    }
    return 0;   /* never reached */
}