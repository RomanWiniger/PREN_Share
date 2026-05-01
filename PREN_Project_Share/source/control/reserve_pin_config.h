#ifndef RESERVE_PIN_CONFIG_H
#define RESERVE_PIN_CONFIG_H

#undef PORT

// Common Definitions
#define _PORT_MKR(x)   PORT##x
#define PORT_MKR(x)    _PORT_MKR(x)
#define _GPIO_MKR(x)   GPIO##x
#define GPIO_MKR(x)    _GPIO_MKR(x)

//////////////////////////////////////////////////////
// RESERVE PINS
//////////////////////////////////////////////////////

#define RES1_REG					13		// USR Config
#define RES1_PORT					A		// USR Config
#define RES1_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES1_PULLUP					(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES1_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES1_PORT_NAME 				PORT_MKR(RES1_PORT)
#define RES1_GPIO_NAME 				GPIO_MKR(RES1_PORT)
#define RES1_MUX_GPIO()     		(RES1_PORT_NAME->PCR[RES1_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES1_PULLUP)|PORT_PCR_PE(RES1_PULL_ENABLE)|PORT_PCR_DSE(RES1_DSE))
#define RES1_SET_OUTPUT()  			(RES1_GPIO_NAME->PDDR|=(1<<RES1_REG))
#define RES1_GPIO_LOW()  			(RES1_GPIO_NAME->PCOR=(1<<RES1_REG))
#define RES1_GPIO_HIGH() 			(RES1_GPIO_NAME->PSOR=(1<<RES1_REG))
#define RES1_STATUS() 				(RES1_GPIO_NAME->PDOR&(1<<RES1_REG))

#define RES2_REG					12		// USR Config
#define RES2_PORT					A		// USR Config
#define RES2_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES2_PULLUP					(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES2_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES2_PORT_NAME 				PORT_MKR(RES2_PORT)
#define RES2_GPIO_NAME 				GPIO_MKR(RES2_PORT)
#define RES2_MUX_GPIO()     		(RES2_PORT_NAME->PCR[RES2_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES2_PULLUP)|PORT_PCR_PE(RES2_PULL_ENABLE)|PORT_PCR_DSE(RES2_DSE))
#define RES2_SET_OUTPUT()  			(RES2_GPIO_NAME->PDDR|=(1<<RES2_REG))
#define RES2_GPIO_LOW()  			(RES2_GPIO_NAME->PCOR=(1<<RES2_REG))
#define RES2_GPIO_HIGH() 			(RES2_GPIO_NAME->PSOR=(1<<RES2_REG))
#define RES2_STATUS() 				(RES2_GPIO_NAME->PDOR&(1<<RES2_REG))

#define RES3_REG					1		// USR Config
#define RES3_PORT					D		// USR Config
#define RES3_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES3_PULLUP					(0) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES3_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES3_PORT_NAME 				PORT_MKR(RES3_PORT)
#define RES3_GPIO_NAME 				GPIO_MKR(RES3_PORT)
#define RES3_MUX_GPIO()     		(RES3_PORT_NAME->PCR[RES3_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES3_PULLUP)|PORT_PCR_PE(RES3_PULL_ENABLE)|PORT_PCR_DSE(RES3_DSE))
#define RES3_SET_OUTPUT()  			(RES3_GPIO_NAME->PDDR|=(1<<RES3_REG))
#define RES3_GPIO_LOW()  			(RES3_GPIO_NAME->PCOR=(1<<RES3_REG))
#define RES3_GPIO_HIGH() 				(RES3_GPIO_NAME->PSOR=(1<<RES3_REG))
#define RES3_STATUS() 				(RES3_GPIO_NAME->PDOR&(1<<RES3_REG))

#define RES4_REG					6		// USR Config
#define RES4_PORT					D		// USR Config
#define RES4_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES4_PULLUP					(1) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES4_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES4_PORT_NAME 				PORT_MKR(RES4_PORT)
#define RES4_GPIO_NAME 				GPIO_MKR(RES4_PORT)
#define RES4_MUX_GPIO()     		(RES4_PORT_NAME->PCR[RES4_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES4_PULLUP)|PORT_PCR_PE(RES4_PULL_ENABLE)|PORT_PCR_DSE(RES4_DSE))
#define RES4_SET_OUTPUT()  			(RES4_GPIO_NAME->PDDR|=(1<<RES4_REG))
#define RES4_GPIO_LOW()  			(RES4_GPIO_NAME->PCOR=(1<<RES4_REG))
#define RES4_GPIO_HIGH() 				(RES4_GPIO_NAME->PSOR=(1<<RES4_REG))
#define RES4_STATUS() 				(RES4_GPIO_NAME->PDOR&(1<<RES4_REG))

#define RES5_REG					7		// USR Config
#define RES5_PORT					D		// USR Config
#define RES5_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES5_PULLUP					(1) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES5_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES5_PORT_NAME 				PORT_MKR(RES5_PORT)
#define RES5_GPIO_NAME 				GPIO_MKR(RES5_PORT)
#define RES5_MUX_GPIO()     		(RES5_PORT_NAME->PCR[RES5_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES5_PULLUP)|PORT_PCR_PE(RES5_PULL_ENABLE)|PORT_PCR_DSE(RES5_DSE))
#define RES5_SET_OUTPUT()  			(RES5_GPIO_NAME->PDDR|=(1<<RES5_REG))
#define RES5_GPIO_LOW()  			(RES5_GPIO_NAME->PCOR=(1<<RES5_REG))
#define RES5_GPIO_HIGH() 				(RES5_GPIO_NAME->PSOR=(1<<RES5_REG))
#define RES5_STATUS() 				(RES5_GPIO_NAME->PDOR&(1<<RES5_REG))

#define RES6_REG					0		// USR Config
#define RES6_PORT					E		// USR Config
#define RES6_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES6_PULLUP					(1) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES6_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES6_PORT_NAME 				PORT_MKR(RES6_PORT)
#define RES6_GPIO_NAME 				GPIO_MKR(RES6_PORT)
#define RES6_MUX_GPIO()     		(RES6_PORT_NAME->PCR[RES6_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES6_PULLUP)|PORT_PCR_PE(RES6_PULL_ENABLE)|PORT_PCR_DSE(RES6_DSE))
#define RES6_SET_OUTPUT()  			(RES6_GPIO_NAME->PDDR|=(1<<RES6_REG))
#define RES6_GPIO_LOW()  			(RES6_GPIO_NAME->PCOR=(1<<RES6_REG))
#define RES6_GPIO_HIGH() 				(RES6_GPIO_NAME->PSOR=(1<<RES6_REG))
#define RES6_STATUS() 				(RES6_GPIO_NAME->PDOR&(1<<RES6_REG))


#define RES7_REG					1		// USR Config
#define RES7_PORT					E		// USR Config
#define RES7_PULL_ENABLE			(1) 	// USR Config		// 1 = Enable, 0 = Disable
#define RES7_PULLUP					(1) 	// USR Config		// 1 = Pullup, 0 = Pulldown
#define RES7_DSE					(1) 	// USR Config		// 1 = Drive Strength enable , 0 = off
#define RES7_PORT_NAME 				PORT_MKR(RES7_PORT)
#define RES7_GPIO_NAME 				GPIO_MKR(RES7_PORT)
#define RES7_MUX_GPIO()     		(RES7_PORT_NAME->PCR[RES7_REG]= PORT_PCR_MUX(1)|PORT_PCR_PS(RES7_PULLUP)|PORT_PCR_PE(RES7_PULL_ENABLE)|PORT_PCR_DSE(RES7_DSE))
#define RES7_SET_OUTPUT()  			(RES7_GPIO_NAME->PDDR|=(1<<RES7_REG))
#define RES7_GPIO_LOW()  			(RES7_GPIO_NAME->PCOR=(1<<RES7_REG))
#define RES7_GPIO_HIGH() 				(RES7_GPIO_NAME->PSOR=(1<<RES7_REG))
#define RES7_STATUS() 				(RES7_GPIO_NAME->PDOR&(1<<RES7_REG))

#endif
