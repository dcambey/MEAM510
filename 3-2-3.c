// main.c





#include "MEAM_general.h" // includes the resources included in the MEAM_general.h file
#include "m_usb.h"

#define PRINTTIME(x) m_usb_tx_uint(x);m_usb_tx_char(10);m_usb_tx_char(13)

void general_setupADC(void){
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);
    //128 prescaler
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);
}

int getAngle(int val){ //retreive the value for the joint
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

float convertAngle(int inputAngle, int joint){
    float out_CW[3] = {2.5,7,25}; //CW max
    float out_CCW[3] = {16,12.5,10}; //CCW max
    float in_CW[3] = {15,191,168}; //CW max
    float in_CCW[3] = {750,605,835}; //CCW max

    float in_CCWise = in_CCW[joint-1];
    float in_CWise = in_CW[joint-1];
    float out_CCWise = out_CCW[joint-1];
    float out_CWise = out_CW[joint-1];

    float duty_cycle = 0;

    if(inputAngle > in_CCWise){
        duty_cycle = out_CCWise;
    } else if(inputAngle < in_CWise){
        duty_cycle = out_CWise;
    } else {
        duty_cycle = out_CWise + ((inputAngle - in_CWise)/(in_CCWise-in_CWise))*(out_CCWise-out_CWise);
    }
    return duty_cycle;
}

void setCommand(float duty_cycle,int joint){
    int compare_val = 0x03FF*(duty_cycle)/100.0; //calculate OCR3A value

   // PRINTTIME(compare_val);

    if(joint == 1){ //if its the thumb
        OCR3A = compare_val;
    }
    if(joint == 2){ //if its the wrist
        OCR1A = compare_val;
    }
    if(joint == 3){ //if its the rotation
        OCR4A = compare_val;
    }
    
}

void setTimers(void){
    //SETUP TIMER 3 for THUMB
    set(DDRC,6); //enable output for OC3A, pin C6 is output

    set(TCCR3B,CS32); //turn timer 3 on, prescaler 256
    clear(TCCR3B,CS31);
    clear(TCCR3B,CS30);

    //up to 0x03FF
    clear(TCCR3B,WGM33);
    set(TCCR3B,WGM32);
    set(TCCR3A,WGM31);
    set(TCCR3A,WGM30);

    //clear at OCR3A, set at rollover
    set(TCCR3A,COM3A1); 
    clear(TCCR3A,COM3A0);

    //SETUP TIMER 1 for WRIST
    set(DDRB,5); //set pin B5 to output (pin 9 on the board)

    set(TCCR1B,CS12); //turn timer 1 on, prescaler 256
    clear(TCCR1B,CS11);
    clear(TCCR1B,CS10);

    //up to 0x03FF
    clear(TCCR1B,WGM13);
    set(TCCR1B,WGM12);
    set(TCCR1A,WGM11);
    set(TCCR1A,WGM10);

     //clear at OCR1A, set at rollover
    set(TCCR1A,COM1A1); 
    clear(TCCR1A,COM1A0);

    //SETUP TIMER 4 for ROTATE
    set(DDRC,7); //set pin C7 to output (pin 13 on board)

    set(TCCR4B,CS43); //turn timer 4 on, prescaler 256
    clear(TCCR4B,CS42);
    clear(TCCR4B,CS41);
    set(TCCR4B,CS40);

    clear(TCCR4D,WGM41); //up to OCR4C
    clear(TCCR4D,WGM40);

    set(TCCR4A,PWM4A);
    set(TCCR4A,COM4A1); //clear at OCR4A, set at 0x00
    set(TCCR4A,COM4A0);

    TC4H = 0x0003; //set 10 bit timer for timer 4 on OCR4C
    OCR4C = 0x00FF;
}

int main(void){

m_usb_init();
_clockdivide(0); //set the clock speed to 16MHz

//SETUP ADC
general_setupADC(); 
//set given pin to disable digital input, in this case ADC7
set(DIDR0,ADC7D); //disable digital inputs for ADC
set(DIDR0,ADC6D);
set(DIDR0,ADC5D);

setTimers();
/* insert your hardware initialization here */

int oldinputAngle[3] = {0,0,0};
int inputAngle = 0;
float duty_cycle = 0;
int i = 1;
int start = 1;

for(;;){

for(i = 1; i < 4; i++) { //for each joint
    inputAngle = getAngle(i+4); //read the potentiometer val through ADC
    if(start < 4){
        start++;
        oldinputAngle[i] = inputAngle+5; //initialize so following if statement runs
    }
    m_usb_tx_string("Angle number ");
    m_usb_tx_uint(i);
    m_usb_tx_string(": ");
    PRINTTIME(inputAngle);
    PRINTTIME(oldinputAngle[i]);//print old angle, for debugging
    if((inputAngle - oldinputAngle[i]) < 30 && (inputAngle - oldinputAngle[i]) > -30){ //if ADC hasnt jumped too far (if consistent)
        
        duty_cycle = convertAngle(inputAngle,i); //update the duty cycle and set torques
        setCommand(duty_cycle,i);
        oldinputAngle[i] = inputAngle; //update the input angle
    }
    
}

m_usb_tx_string("\n");
}

return 0; /* never reached */

}