/*
 * coil.c
 *
 *  Created on: 17.03.2026
 *      Author: roman
 */
#include "coil.h"
#include "coil_config.h"
#include "stdbool.h"
#include "reserve_pin_config.h"

int coilInit(){
	RES1_MUX_GPIO();     // Pin als GPIO konfigurieren
	RES1_SET_OUTPUT();   // Pin als Ausgang setzen

	return 1;
}

int coil_ctrl(bool act){
	if(act){
		RES1_GPIO_HIGH();    // Pin HIGH (3.7V)

	}else{
		RES1_GPIO_LOW();     // Pin LOW (0V)
	}

	return 1;
}
