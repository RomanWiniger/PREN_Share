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

#if DEBUG_MODE

  RES1_MUX_GPIO();
  RES1_SET_OUTPUT();
  RES1_GPIO_HIGH();
  RES2_MUX_GPIO();
  RES2_SET_OUTPUT();

uint64_t cycles = 0;
  while(true){
	  moveWay(-600,300,326);
	  moveWay(2510,10,45);
	  moveWay(2510,30110,-20000);
	  moveWay(-100,500,-456);
	  moveWay(1000,5000,-456);
	  moveWay(-520,550,-456);
	  moveWay(-3324,3145,-3242);
	  moveWay(-100,500,-456);
	  moveWay(500,100,0);
	  moveWay(0,1500,123);
	  moveWay(500,0,600);
	  moveWay(50000,-49080,51100);
	  cycles++;
  }
	  /*
  moveWay(300,500,800);
  moveWay(-3000,-1000,1067);
  moveWay(120,456,-1000);
*/
  //BitMonitor PORTA
  PORTA->PCR;
  GPIOA->PDDR;
  GPIOA->PDOR;

  //BitMonitor PORTB
  PORTB->PCR;
  GPIOB->PDDR;
  GPIOB->PDOR;

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

