/*
 * dc_motor.h
 *
 *  Created on: Oct 5, 2022
 *      Author: User
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include"std_types.h"

typedef enum{
	STOP, CW, CCW
}DCMotor_State;

void DCMotor_init(void);
void DcMotor_Rotate(DCMotor_State state, uint8 speed);



#endif /* DC_MOTOR_H_ */
