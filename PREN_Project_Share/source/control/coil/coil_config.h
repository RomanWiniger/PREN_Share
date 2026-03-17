/*
 * coil_config.h
 *
 *  Created on: 17.03.2026
 *      Author: roman
 */

#ifndef COIL_CONFIG_H_
#define COIL_CONFIG_H_

#include "platform.h"

#define ENABLE_COIL		1

#if ENABLE_COIL
#define CTRL_COIL_PORT      	PORTD
#define CTRL_COIL_REG      		(0)
#define CTRL_COIL_GPIO()     	((CTRL_COIL_PORT) ->PCR[CTRL_COIL_REG] = PORT_PCR_MUX(1)|PORT_PCR_PS(0)|PORT_PCR_PE(1)|PORT_PCR_DSE(1))
#define CTRL_COIL_ACT()     	(GPIOB->PSOR=(1<<CTRL_COIL_REG))
#define CTRL_COIL_DEACT()     	(GPIOB->PCOR=(1<<CTRL_COIL_REG))
#define	CTRL_COIL_OUTPUT()		(GPIOB->PDDR|=(1<<CTRL_COIL_REG))

#endif


#endif /* COIL_CONFIG_H_ */
