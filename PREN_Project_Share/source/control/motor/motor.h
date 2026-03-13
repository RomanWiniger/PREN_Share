/*
 * stepper.h
 *
 *  Created on: 27.02.2026
 *      Author: roman winiger
 */

#ifndef MOTOR_STEPPER_H_
#define MOTOR_STEPPER_H_

void motorInit(void);
int moveWay(uint32_t Mot1, uint32_t Mot2,uint32_t Mot3);
int moveRotation(uint32_t RotSteps);

#endif /* MOTOR_STEPPER_H_ */
