

/*
 * main.c
 *
 *  Created on: Sep 14, 2023
 *      Author: Shorouk Shawky
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// global variables
unsigned char FirstDigitSec=0;
unsigned char SecondDigitSec=0;
unsigned char FirstDigitMin=0;
unsigned char SecondDigitMin=0;
unsigned char FirstDigitHr=0;
unsigned char SecondDigitHr=0;

ISR(TIMER1_COMPA_vect){

	if (FirstDigitSec==9){
		FirstDigitSec=0;
		SecondDigitSec++;
	}else{
		FirstDigitSec++;}
	if (FirstDigitHr==3 && SecondDigitHr==2 && SecondDigitMin==5 && FirstDigitMin==9 && FirstDigitSec==9 && SecondDigitSec==5){
		FirstDigitSec=0;
		SecondDigitSec=0;
		FirstDigitMin=0;
		SecondDigitMin=0;
	    FirstDigitHr=0;
		SecondDigitHr=0;
	}
	if((FirstDigitMin==9)&&(SecondDigitMin==5)&&(SecondDigitSec==5)&&(FirstDigitSec==9)){
			FirstDigitHr++;
			FirstDigitMin=0;
			SecondDigitMin=0;
			}

	if ((FirstDigitHr==9)&&(SecondDigitMin==5)&&(FirstDigitMin==9)&&(SecondDigitSec==5)&&(FirstDigitSec==9)){
		FirstDigitHr=0;
		FirstDigitMin=0;
		SecondDigitMin=0;
		FirstDigitSec=0;
		SecondDigitSec=0;
		SecondDigitHr++;
	}



	if((SecondDigitSec==5)&&(FirstDigitSec==9)){
			FirstDigitMin++;
			FirstDigitSec=0;
			SecondDigitSec=0;}

	if((FirstDigitMin==9)&&(SecondDigitSec==5)&&(FirstDigitSec==9)){
		FirstDigitMin=0;
		SecondDigitMin++;}





}
/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	PORTA=0x3F;
	PORTC=0;
	FirstDigitSec=0;
	SecondDigitSec=0;
	FirstDigitMin=0;
	SecondDigitMin=0;
	FirstDigitHr=0;
	SecondDigitHr=0;
	TCCR1B|=(1<<CS10)|(1<<CS12);
	}

ISR(INT1_vect){
	TCCR1B&=~(1<<CS10)&(~(1<<CS11))&(~(1<<CS12));
}
ISR(INT2_vect){
	TCCR1B|=(1<<CS10)|(1<<CS12);
}

//function difinition
void Timer_CTC_init(void);
void EX_INT0_WithFallingEdge(void);
void EX_INT1_WithRaisingEdge(void);
void EX_INT2_WithFallingEdge(void);

int main(void){

	DDRC|=(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC0); // DECODER pin as output

	DDRA|= 0x3F; // enable pin of 7 segment as output
	PORTC=0;
	PORTA=0x3F;

	SREG|=(1<<7);

	EX_INT0_WithFallingEdge();
	EX_INT1_WithRaisingEdge();
	EX_INT2_WithFallingEdge();
	Timer_CTC_init();


	while(1){
		PORTA =0x01;
		PORTC=((PORTC&(0xF0))|(FirstDigitSec&0x0F));
		_delay_ms(4);


		PORTA =0x02;
		PORTC=((PORTC&(0xF0))|(SecondDigitSec&0x0F));
//		PORTA =0x30;
		_delay_ms(4);

		PORTA=0x04;
		PORTC=((PORTC&(0xF0))|(FirstDigitMin&0x0F));
//		PORTA =0x38;
		_delay_ms(4);

		PORTA=0x08;
		PORTC=((PORTC&(0xF0))|(SecondDigitMin&0x0F));
//		PORTA=0x3C;
		_delay_ms(4);

		PORTA=0x10;
		PORTC=((PORTC&(0xF0))|(FirstDigitHr&0x0F));
//		PORTA=0x3E;
		_delay_ms(4);

		PORTA=0x20;
		PORTC=((PORTC&(0xF0))|(SecondDigitHr&0x0F));
//		PORTA=0x3F;
		_delay_ms(4);

	}
}
void Timer_CTC_init(void){
	TCNT1=0;
	OCR1A=1000;

	TIMSK|=(1<<OCIE1A);    // enable Output Compare A Match Interrupt

	TCCR1A|=(1<<FOC1A);
	TCCR1B|=(1<<WGM12)|(1<<CS10) |(1<<CS12);    //prescale equal 1024 and waveform generation for mode 4

}
void EX_INT0_WithFallingEdge(void){
	 DDRD  &= (~(1<<PD2));
	 PORTD |=(1<<PD2);
	 MCUCR|=(1<<ISC01);
	 MCUCR&=~(1<<ISC00);
	 GICR|=(1<<INT0);
	 SREG|=(1<<7);
}

void EX_INT1_WithRaisingEdge(void){
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin

	MCUCR |= (1<<ISC11)|(1<<ISC10);    // Trigger INT1 with the raising edge

	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit

}

void EX_INT2_WithFallingEdge(void){
	DDRB   &= (~(1<<PB2));   // Configure INT2/PB2 as input pin
	PORTB |=(1<<PB2);
	MCUCSR &= ~(1<<ISC2);     // Trigger INT2 with the falling edge
	GICR   |= (1<<INT2);	 // Enable external interrupt pin INT2
	SREG   |= (1<<7);        // Enable interrupts by setting I-bit
}

