/*
 * coil.c
 *
 *  Created on: 17.03.2026
 *      Author: roman
 */
#include "coil.h"
#include "coil_config.h"
#include "stdbool.h"

int coilInit(){
	CTRL_COIL_GPIO();
	CTRL_COIL_OUTPUT();

	return 1;
}

int coil_ctrl(bool act){
	if(act){
		CTRL_COIL_ACT();
	}else{
		CTRL_COIL_DEACT();
	}

	return 1;
}
