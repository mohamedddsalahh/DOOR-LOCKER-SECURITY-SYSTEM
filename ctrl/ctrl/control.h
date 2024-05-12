/*
 * ctrl.h
 *
 *  Created on: Nov 2, 2022
 *      Author: USER
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include"std_types.h"

#define PASS_READY		0x01
#define	CONFIRM_READY	0x02
#define MC2_READY	    0x03
#define CHECK			0x04
#define	MC1_READY		0x05

#define OPTION			0x06



#define PASS_ADDRESS		0x0311
#define	RECONFIRM_ADDRESS	0x0339

void create_pass(void);
void EEPROM_WRITE_PASS(uint8 arr[]);
void EEPROM_WRITE_CONFRIM(uint8 arr[]);
void option_processing(void);

void motorTimer(void);

#endif /* CONTROL_H_ */
