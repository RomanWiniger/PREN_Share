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
//     ------------------------------------------------
//     \author  Pascal Hofstetter, Roman Winiger
//     \date    18.04.2026


#include <stdio.h>
#include "platform.h"
#include "uart.h"
#include "term.h"
#include "control.h"
#include "globals.h"
#include "coil.h"
#include "motor.h"
#include "wait.h"
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



	  moveToInitPos(500);

#endif

#if DEBUG_MODE

  // Initialize Reserve Pins (ISR Monitoring)
  RES1_MUX_GPIO();
  RES1_SET_OUTPUT();
  RES2_MUX_GPIO();
  RES2_SET_OUTPUT();

  //moveToInitPos(200);

  while (true) {
      // move 1: piece=1 source_mm=(49.6, 142.5) dest_mm=(34.5, 29.2)
      moveWay(2926, 2484, 2451, 0, 0);
      moveWay(370, -1803, 2204, 1, 0);
      moveWay(713, 798, 514, 1, 0);
      moveWay(598, 658, 452, 1, 0);
      moveWay(-1312, -1456, -966, 1, 0);
      moveWay(2789, -615, -2042, 1, 1);
      moveWay(983, 1532, 1205, 1, 0);
      moveWay(-983, -1532, -1205, 0, 0);

      // move 2: piece=0 source_mm=(234.1, 154.4) dest_mm=(95.6, 33.3)
      moveWay(0, 0, 0, 0, 0);
      moveWay(-2582, 4271, -2032, 0, 0);
      moveWay(698, 540, 797, 1, 0);
      moveWay(586, 471, 657, 1, 0);
      moveWay(-1284, -1010, -1454, 1, 0);
      moveWay(2578, -3038, 545, 1, 1);
      moveWay(979, 1379, 1386, 1, 0);
      moveWay(-979, -1379, -1386, 0, 0);

      // move 3: piece=4 source_mm=(69.8, 64.9) dest_mm=(158.8, 31.2)
      moveWay(0, 0, 0, 0, 0);
      moveWay(-4566, 1066, 4173, 0, 0);
      moveWay(840, 679, 480, 1, 0);
      moveWay(690, 574, 428, 1, 0);
      moveWay(-1531, -1253, -908, 1, 0);
      moveWay(4644, 471, -5447, 1, 1);
      moveWay(976, 1192, 1544, 1, 0);
      moveWay(-976, -1192, -1544, 0, 0);

      // move 4: piece=2 source_mm=(139.4, 134.2) dest_mm=(29.0, 94.5)
      moveWay(0, 0, 0, 0, 0);
      moveWay(-3400, -384, 2841, 0, 0);
      moveWay(751, 666, 640, 1, 0);
      moveWay(624, 563, 544, 1, 0);
      moveWay(-1375, -1229, -1185, 1, 0);
      moveWay(5133, -2751, -181, 1, 1);
      moveWay(827, 1567, 1233, 1, 0);
      moveWay(-827, -1567, -1233, 0, 0);

      // move 5: piece=5 source_mm=(150.3, 56.2) dest_mm=(90.8, 98.5)
      moveWay(0, 0, 0, 0, 0);
      moveWay(-6763, 4282, 1259, 0, 0);
      moveWay(869, 566, 573, 1, 0);
      moveWay(710, 492, 496, 1, 0);
      moveWay(-1579, -1058, -1069, 1, 0);
      moveWay(6747, -3040, -2782, 1, 1);
      moveWay(823, 1427, 1408, 1, 0);
      moveWay(-823, -1427, -1408, 0, 0);

      // move 6: piece=3 source_mm=(241.5, 72.0) dest_mm=(155.2, 93.2)
      moveWay(0, 0, 0, 0, 0);
      moveWay(-6125, 4707, 876, 0, 0);
      moveWay(824, 468, 706, 1, 0);
      moveWay(679, 418, 594, 1, 0);
      moveWay(-1503, -886, -1301, 1, 0);
      moveWay(6089, -3140, -2193, 1, 1);
      moveWay(830, 1246, 1559, 1, 0);
      moveWay(-830, -1246, -1559, 0, 0);

      waitMs(1000);
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
  moveToInitPos(300);
  while(TRUE)
    {
      termDoWork();//lauscht an der UART Verbindung
    }

#endif

  // Never leave main
  for (;;){}
  return 0;
}

