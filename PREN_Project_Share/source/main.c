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
#if DEBUG_MODE

	#include "motor.h"
#endif



/**
 * Terminal-Demo
 */
int main(void)
{

  termInit(57600);
  commandInit();
  controlInit();

#if DEBUG_MODE

  moveWay(-3000,5000,2000);
  moveWay(3000,-5000,2000);
  moveWay(6000,5000,-3000);

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
  while(TRUE)
  {
    termDoWork();//lauscht an der UART Verbindung
  }

  // Never leave main
  for (;;){}
  return 0;
}

