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
  termInit(57600);
  commandInit();
  controlInit();

  MOTOR1_STEP_GPIO();

#if DEBUG_MODE

  moveWay(-300,500,800);
  moveWay(2000,-1000,1567);
  moveWay(12,456,-987);

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

