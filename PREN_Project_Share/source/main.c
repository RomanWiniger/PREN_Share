//      ____  ____  ________   __
//     |  _ \|  _ \| ____|  \ | |
//     | |_) | |_) |  _| | | \| |
//	   |  __/|  _ <| |___| |\ \ |
//     |_|   |_| \_\_____|_| \__|

//      ____       _ _             ____       _           _
//     |  _ \  ___| | |_ __ _     |  _ \ ___ | |__   ___ | |_ ___ _ __
//     | | | |/ _ \ | __/ _` | __ | |_) / _ \| '_ \ / _ \| __/ _ \ '__|
//     | |_| |  __/ | || (_| ||__||  _ < (_) | |_) | (_) | ||  __/ |
//     |____/ \___|_|\__\__,_|    |_| \_\___/|_.__/ \___/ \__\___|_|
//
//      (c) FS 2026 Gruppe 1 PREN
//		Hochschule Luzern T&A  ==== www.hslu.ch ====
//
//     \brief   Delta-Roboter Controller
//     \author  Pascal Hofstetter, Roman Winiger
//     \date    18.04.2026
//     ------------------------------------------------


#include <stdio.h>
#include "platform.h"
#include "uart.h"
#include "term.h"
#include "control.h"
#include "globals.h"
#include "coil.h"

#if SENSOR_TEST
	#include "sensor_config.h"
#endif

#if DEBUG_MODE
	#include "motor_config.h"
	#include "motor.h"
#endif

#if INIT_POS_TEST
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

  coil_ctrl(true);
  coil_ctrl(false);
#if SENSOR_TEST

	// sensorInit() is done in controlInit()
	bool sens1 = false;
	bool sens2 = false;
	bool sens3 = false;
	for(;;){
	if(SENSOR1_STATUS())
	{sens1=true;}
	else{sens1=false;}
	if(SENSOR2_STATUS())
	{sens2=true;}
	else{sens2=false;}
	if(SENSOR3_STATUS())
	{sens3=true;}
	else{sens3=false;}

}
#endif

#if INIT_POS_TEST
	// motorInit() is done in controlInit()
	moveToInitPos(1000); // Argument: Mikro-sekunden zwischen Step-Toggles
						// je tiefer je schneller
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
  RES2_MUX_GPIO();
  RES2_SET_OUTPUT();

  while(true){
	  moveWay(2467, 2668, 2320);
	  moveWay(-2467, -2668, -2320);
	  moveWay(-3172, 4036, 243);
	  moveWay(2845, 2476, 2476);
	  moveWay(-2845, -2476, -2476);
	  moveWay(2810, -2366, -2366);
	  moveWay(-4878, -4878, -4878);

  }

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

