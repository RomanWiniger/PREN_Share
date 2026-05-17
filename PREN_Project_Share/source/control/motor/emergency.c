/*
 * emergency.c
 *
 *  Created on: 15.05.2026
 *      Author: roman
 */

#include "platform.h"
#include "emergency.h"

void emergencyStop_Init(){

PORTA->PCR[2]= PORT_PCR_MUX(3)|PORT_PCR_PS(0)|PORT_PCR_PE(0)|PORT_PCR_DSE(0);
FTM0->CONTROLS[7].CnSC =
		FTM_CnSC_CHIE(1) |	// Interrupt Enable
		FTM_CnSC_ELSx(1) |  // Capture on Rising Edge
		FTM_CnSC_MSx(0);	// 0 = Input Capture

// Nested Vector Activation wird im ftm0EnableIRQ gemacht.
}
