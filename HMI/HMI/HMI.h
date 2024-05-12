/*
 * HMI.h
 *
 *  Created on: Nov 2, 2022
 *      Author: USER
 */

#ifndef HMI_H_
#define HMI_H_

#include"std_types.h"



#define PASS_READY		0x01
#define	CONFIRM_READY	0x02
#define MC2_READY 		0x03
#define CHECK			0x04
#define	MC1_READY		0x05

#define OPTION			0x06


void create_pass(void);
void send_pass(uint8 arr[]);
void send_confirm(uint8 arr[]);
void option_screen(void);

void Door_Locking(void);
void Door_Unlocking(void);
void messageTimer(void);

#endif /* HMI_H_ */
