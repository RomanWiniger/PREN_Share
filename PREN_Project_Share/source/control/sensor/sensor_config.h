/*
 * coil_config.h
 *
 *  Created on: 17.03.2026
 *      Author: roman
 */

#ifndef SENSOR_CONFIG_H_
#define SENSOR_CONFIG_H_

#include "platform.h"


#define ENABLE_SENSOR			1

// Common Definitions
#define _PORT_MKR(x)   PORT##x
#define PORT_MKR(x)    _PORT_MKR(x)
#define _GPIO_MKR(x)   GPIO##x
#define GPIO_MKR(x)    _GPIO_MKR(x)


#if ENABLE_SENSOR

#define SENSOR1_PORT				C		// USR Config
#define SENSOR1_REG					8		// USR Config
#define SENSOR1_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define SENSOR1_PULLUP				(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define SENSOR1_PORT_NAME 			PORT_MKR(SENSOR1_PORT)
#define SENSOR1_GPIO_NAME 			GPIO_MKR(SENSOR1_PORT)
#define SENSOR1_MUX_GPIO()     		(SENSOR1_PORT_NAME->PCR[SENSOR1_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(SENSOR1_PULLUP)|PORT_PCR_PE(SENSOR1_PULL_ENABLE))
#define SENSOR1_SET_INPUT()  		(SENSOR1_GPIO_NAME->PDDR&=~(1<<SENSOR1_REG))
#define SENSOR1_STATUS() 			(SENSOR1_GPIO_NAME->PDIR&(1<<SENSOR1_REG))

#define SENSOR2_PORT				C		// USR Config
#define SENSOR2_REG					9		// USR Config
#define SENSOR2_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define SENSOR2_PULLUP				(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define SENSOR2_PORT_NAME 			PORT_MKR(SENSOR2_PORT)
#define SENSOR2_GPIO_NAME 			GPIO_MKR(SENSOR2_PORT)
#define SENSOR2_MUX_GPIO()     		(SENSOR2_PORT_NAME->PCR[SENSOR2_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(SENSOR2_PULLUP)|PORT_PCR_PE(SENSOR2_PULL_ENABLE))
#define SENSOR2_SET_INPUT()  		(SENSOR2_GPIO_NAME->PDDR&=~(1<<SENSOR2_REG))
#define SENSOR2_STATUS() 			(SENSOR2_GPIO_NAME->PDIR&(1<<SENSOR2_REG))

#define SENSOR3_PORT				C		// USR Config
#define SENSOR3_REG					10		// USR Config
#define SENSOR3_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define SENSOR3_PULLUP				(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define SENSOR3_PORT_NAME 			PORT_MKR(SENSOR3_PORT)
#define SENSOR3_GPIO_NAME 			GPIO_MKR(SENSOR3_PORT)
#define SENSOR3_MUX_GPIO()     		(SENSOR3_PORT_NAME->PCR[SENSOR3_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(SENSOR3_PULLUP)|PORT_PCR_PE(SENSOR3_PULL_ENABLE))
#define SENSOR3_SET_INPUT()  		(SENSOR3_GPIO_NAME->PDDR&=~(1<<SENSOR3_REG))
#define SENSOR3_STATUS() 			(SENSOR3_GPIO_NAME->PDIR&(1<<SENSOR3_REG))

#endif


#endif /* SENSOR_CONFIG_H_ */
