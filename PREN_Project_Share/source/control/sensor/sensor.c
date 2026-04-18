/*
 * coil.c
 *
 *  Created on: 17.03.2026
 *      Author: roman
 */
#include <sensor/sensor.h>
#include <sensor/sensor_config.h>
#include "stdbool.h"

void sensorInit(){
	SENSOR1_MUX_GPIO();
	SENSOR2_MUX_GPIO();
	SENSOR3_MUX_GPIO();

	SENSOR1_SET_INPUT();
	SENSOR2_SET_INPUT();
	SENSOR3_SET_INPUT();
}

