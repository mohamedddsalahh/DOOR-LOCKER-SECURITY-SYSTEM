/*
 * buzzer.c
 *
 *  Created on: Nov 1, 2022
 *      Author: USER
 */

#include"gpio.h"
#include"buzzer.h"
#include"std_types.h"


void Buzzer_init(void){
	GPIO_setupPinDirection(PORTC_ID, PIN5_ID, PIN_OUTPUT);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);			//Enable Pin
}

void Buzzer_on(void){
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_HIGH);
}
void Buzzer_off(void){
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
}


