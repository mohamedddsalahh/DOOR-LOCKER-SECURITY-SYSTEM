/*
 * timer1.c
 *
 *  Created on: Nov 1, 2022
 *      Author: USER
 */
#include"std_types.h"
#include"timer1.h"
#include<avr/io.h>
#include<avr/interrupt.h>
#include"common_macros.h"

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application*/
		(*g_callBackPtr)();
	}
}


ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application*/
		(*g_callBackPtr)();
	}
}

void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	TCCR1A = (1<<FOC1A) | (((Config_Ptr->mode)& 0b0011)<< WGM10);
	TCCR1B = ((((Config_Ptr->mode)& 0b1100)>>2)<< WGM12) | ((Config_Ptr->prescaler)<<CS10);
	TCNT1 = (Config_Ptr->initial_value);		/* Set timer1 initial count to initial value */

	SREG |= (1<<7);

	switch((Config_Ptr->mode)){
	case NORMAL:
		TIMSK |= (1<<TOIE1); /* Enable Timer1 Overflow Interrupt */
		break;
	case CTC:
		OCR1A = (Config_Ptr->compare_value);		/* Set the Compare value */
		TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */
		break;
	}
}

void Timer1_deInit(void){
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	/* Disable the Timer1 Compare interrupt */
	TIMSK &= ~(1<<OCIE1A);
}

void Timer1_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
