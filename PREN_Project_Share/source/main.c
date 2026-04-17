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

