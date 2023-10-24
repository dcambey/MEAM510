#include "MEAM_general.h"
#include "m_usb.h"
/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#define PRINTTIME(x) m_usb_tx_uint(x);m_usb_tx_char(10);m_usb_tx_char(13)

general_setupADC(void){
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);
    //128 prescaler
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);
}

int get_value(int val){
    if(val & (1 << 0)){ //if the binary of val has one in first bit
        set(ADMUX,MUX0);
    } else {
    clear(ADMUX,MUX0);
    }
      if(val & (1 << 1)){ //if the binary of val has one in second bit
        set(ADMUX,MUX1);
    } else {
        clear(ADMUX,MUX1);
    }
        if(val & (1 << 2)){ //if the binary of val has one in third bit
        set(ADMUX,MUX2);
    } else {
        clear(ADMUX,MUX2);
    }
        if(val & (1 << 3)){ //if the binary of val has one in fourth bit
        set(ADCSRB,MUX5);
    } else {
        clear(ADCSRB,MUX5);
    }

    set(ADCSRA,ADEN); //enable ADC
    set(ADCSRA,ADSC); //start ADC

    while(bit_is_clear(ADCSRA,ADIF)); //wait for the ADC to be ready
    clear(ADCSRA,ADIF); //prep to redo ADC
    set(ADCSRA,ADSC);
    return ADC;
}

int main(void)
{
    m_usb_init();
    general_setupADC(); 
    //set given pin to disable digital input, in this case ADC7
    set(DIDR0,ADC7D); //disable digital input
    set(DIDR0,ADC6D);
    set(DIDR0,ADC5D);
    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        int val = get_value(7);
        m_usb_tx_string("Rotation Angle: ");
        PRINTTIME(val);
        
        val = get_value(6);
        m_usb_tx_string("Wrist Angle: ");
        PRINTTIME(val);

        val = get_value(5);
        m_usb_tx_string("Thumb Angle: ");
        PRINTTIME(val);

        m_usb_tx_string("\n");
        _delay_ms(1000);
        // m_usb_tx_char("third value: ")
        // val = get_value(8);
        // PRINTTIME(val);
    }
    return 0;   /* never reached */
}