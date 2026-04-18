//         __  ___   ______   ______   __  __    _   __
//        /  |/  /  / ____/  / ____/  / / / /   / | / /
//       / /|_/ /  / /      / /_     / / / /   /  |/ /
//      / /  / /  / /___   / __/    / /_/ /   / /|  /
//     /_/  /_/   \____/  /_/       \____/   /_/ |_/
//     (c) Hochschule Luzern T&A  ==== www.hslu.ch ====
//
//     \brief   Exercise 06 - UART
//     \author  Christian Jost, christian.jost@hslu.ch
//     \date    24.03.2025
//     ------------------------------------------------


#include <stdio.h>
#include "platform.h"
#include "uart.h"
#include "term.h"
#include "control.h"
#include "globals.h"
#include "motor_config.h"

#if SENSOR_TEST
#include "sensor.h"
#include "sensor_config.h"
#endif

#if DEBUG_MODE
	#include "motor.h"
#endif



/**
 * Terminal-Demo
 */
int main(void)
{
  termInit(115200);
  commandInit();
  controlInit();

#if SENSOR_TEST
  bool sensor1,sensor2,sensor3;

  for(;;){
  	  if(SENSOR1_STATUS()){sensor1=true;}else{{sensor1=false;}}
  	  if(SENSOR2_STATUS()){sensor2=true;}else{{sensor3=false;}}
  	  if(SENSOR3_STATUS()){sensor3=true;}else{{sensor3=false;}}
  }
#endif

#if TEST_SEQUENCE
  uint64_t cycles = 0;
  while(true){
	  moveWay(4878, 4878, 4878);
	  moveWay(361, -1670, 2123);
	  moveWay(2467, 2668, 2320);
	  moveWay(-2467, -2668, -2320);
	  moveWay(-3172, 4036, 243);
	  moveWay(2845, 2476, 2476);
	  moveWay(-2845, -2476, -2476);
	  moveWay(2810, -2366, -2366);
	  moveWay(-4878, -4878, -4878);
	  cycles++;
  }
#endif

#if DEBUG_MODE
  // Initialize Reserve Pins (ISR Monitoring)
  RES1_MUX_GPIO();
  RES1_SET_OUTPUT();
  RES1_GPIO_HIGH();
  RES2_MUX_GPIO();
  RES2_SET_OUTPUT();


  //BitMonitor PORTA
  PORTA->PCR;
  GPIOA->PDDR;
  GPIOA->PDOR;

  //BitMonitor PORTB
  PORTB->PCR;
  GPIOB->PDDR;
  GPIOB->PDOR;

  //BitMonitor PORTC
  PORTC->PCR;
  GPIOC->PDDR;
  GPIOC->PDOR;

  //BitMonitor PORTD
  PORTD->PCR;
  GPIOD->PDDR;
  GPIOD->PDOR;

#else
  while(TRUE)
    {
      termDoWork();//lauscht an der UART Verbindung
    }

#endif

  // Never leave main
  for (;;){}
  return 0;
}

