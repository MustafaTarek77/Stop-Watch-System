/*
 * StopWatch.c
 *
 *  Created on: Sep 17, 2022
 *      Author: Mustafa Tarek
 */

#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>

unsigned char num1=0; //Seconds
unsigned char num2=0; //Minutes
unsigned char num3=0; //Hours

void TIMER1_Init(void)
{
	/*
	 Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=0
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A);

	/*
	 Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. Prescaler = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);

	/* Enable Timer1 Compare A Interrupt */
	TIMSK |= (1<<OCIE1A);

	/* Set timer1 initial count to zero */
	TCNT1 = 0;

	/* Set the Compare value */
	OCR1A = 980;
}


void INT0_Init(void)
{
	DDRD&=~(1<<PD2);
	PORTD|=(1<<PD2);
	//Falling edge
	MCUCR|=(1<<ISC01);
	GICR|=(1<<INT0);
}

void INT1_Init(void)
{
	DDRD&=~(1<<PD3);
	//Raising edge
	MCUCR|=(1<<ISC11)|(1<<ISC10);
	GICR|=(1<<INT1);
}

void INT2_Init(void)
{
	DDRB&=~(1<<PB2);
	PORTB|=(1<<PB2);
	//Falling edge
	GICR|=(1<<INT2);
}

ISR(TIMER1_COMPA_vect)
{
	if(num1==59)  //check seconds
		if(num2==59)  //check minutes
			if(num3==23)  //check hours
			{
				num1=0;
				num2=0;
				num3=0;
			}
			else
			{
				num3++;
				num2=0;
				num1=0;
			}
		else
		{
			num2++;
			num1=0;
		}
	else
		num1++;
}

ISR(INT0_vect)
{
	num1=0;
	num2=0;
	num3=0;
}

ISR(INT1_vect)
{
	TCCR1B=0;
}

ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}

int main(void)
{
	DDRC|=0x0f;
	PORTC&=~0x0f;
	DDRA|=0x3f;
	PORTA&=0x00;

	/*Enable interrupts*/
	SREG|=(1<<7);

	TIMER1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	while(1)
	{
		// Display the required number on the 7-segment
		PORTA=(1<<0);
		PORTC = (PORTC & 0xF0) | ((num1%10) & 0x0F);
		_delay_us(50);
		PORTA=(1<<1);
		PORTC = (PORTC & 0xF0) | ((num1/10) & 0x0F);
		_delay_us(50);
		PORTA=(1<<2);
		PORTC = (PORTC & 0xF0) | ((num2%10) & 0x0F);
		_delay_us(50);
		PORTA=(1<<3);
		PORTC = (PORTC & 0xF0) | ((num2/10) & 0x0F);
		_delay_us(50);
		PORTA=(1<<4);
		PORTC = (PORTC & 0xF0) | ((num3%10) & 0x0F);
		_delay_us(50);
		PORTA=(1<<5);
		PORTC = (PORTC & 0xF0) | ((num3/10) & 0x0F);
		_delay_us(50);
	}
}


