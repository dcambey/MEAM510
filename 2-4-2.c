#include "MEAM_general.h"
#include "m_usb.h"
/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#define PRINTTIME(x) m_usb_tx_uint(x);m_usb_tx_char(10);m_usb_tx_char(13)

int oldVal = 0;
int counts_el = 0;
int freq_mult = 16000000/1024;

void dolights(){
    PRINTTIME(freq_mult/counts_el);
    // clear(PORTB,6);
    // clear(PORTB,5);
   if(counts_el > freq_mult*0.8/(662) && counts_el < freq_mult*1.2/(662)){
            //turn on green led
            set(PORTB,5);
            clear(PORTB,6);
            counts_el = 0;
            _delay_ms(500);
        }
    else if(counts_el > freq_mult*0.8/50 && counts_el < freq_mult*1.2/50){
            //turn on red led
            set(PORTB,6);
            clear(PORTB,5);
            counts_el = 0;
            _delay_ms(500);
        }
    else {
        clear(PORTB,6);
        clear(PORTB,5);
        counts_el = 0;
    }
}

void calcFreq(){
    while(!bit_is_set(TIFR3,ICF3)) dolights();  //if input capture happened
        set(TIFR3,ICF3);
        counts_el = ICR3 - oldVal; //twos complement works, how many counts passed
        
        // PRINTTIME(counts_el*1024*1000/(16*10^6)); //print time since last depressed
        oldVal = ICR3; //set currrent input capture value as the old value
        }
   

int main(void)
{
    m_usb_init();
    _clockdivide(0); //set the clock speed to 16Mhz
    clear(DDRC,7); // set pin C7 to input

    set(DDRB,6); //set pin B6 to output, red LED
    set(DDRB,5); //set pin B5 to outpupt, green LED

    //set timer with prescaler of 1024
    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30);

    //set up input capture
    set(TCCR3B,ICES3); //store on rising edge

    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        calcFreq();
    }
    return 0;   /* never reached */
}