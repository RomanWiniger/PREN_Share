/*
 * stepper.h
 *
 *  Created on: 27.02.2026
 *      Author: roman winiger
 */

#ifndef MOTOR_STEPPER_H_
#define MOTOR_STEPPER_H_

void motorInit(void);
int moveWay(int32_t Mot1, int32_t Mot2,int32_t Mot3);
int moveRotation(int32_t RotSteps);
void moveToInitPos(void);

#endif /* MOTOR_STEPPER_H_ */
