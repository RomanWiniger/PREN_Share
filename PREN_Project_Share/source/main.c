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
#if DEBUG_MODE
	#include "wait.h"
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

  while (true){
	  // move 1: piece=3 source_mm=(68.2, 54.0) dest_mm=(30.9, 32.2)
	  moveWay(-1091, 887, -2677, 0);
	  moveWay(-675, -999, -649, 0);
	  moveWay(-3854, 2847, 1555, 0);
	  moveWay(853, 667, 470, 0);
	  moveWay(699, 566, 420, 1);
	  moveWay(-1552, -1233, -890, 1);
	  moveWay(4964, -2603, -2853, 1);
	  moveWay(512, 849, 648, 1);
	  moveWay(451, 696, 552, 1);
	  moveWay(-964, -1545, -1200, 0);

	  // move 2: piece=5 source_mm=(233.8, 58.7) dest_mm=(87.1, 35.1)
	  moveWay(0, 0, 0, 0);
	  moveWay(-4840, 5737, -415, 0);
	  moveWay(844, 466, 680, 0);
	  moveWay(693, 418, 576, 1);
	  moveWay(-1538, -884, -1256, 1);
	  moveWay(4794, -4618, -966, 1);
	  moveWay(512, 769, 746, 1);
	  moveWay(451, 638, 621, 1);
	  moveWay(-963, -1407, -1367, 0);

	  // move 3: piece=1 source_mm=(243.2, 147.2) dest_mm=(152.0, 30.4)
	  moveWay(0, 0, 0, 0);
	  moveWay(-2855, 3569, -743, 0);
	  moveWay(707, 521, 800, 0);
	  moveWay(593, 458, 660, 1);
	  moveWay(-1300, -979, -1460, 1);
	  moveWay(2830, -2000, -582, 1);
	  moveWay(516, 657, 841, 1);
	  moveWay(454, 558, 690, 1);
	  moveWay(-970, -1215, -1531, 0);

	  // move 4: piece=0 source_mm=(152.0, 152.0) dest_mm=(25.6, 96.8)
	  moveWay(0, 0, 0, 0);
	  moveWay(-3037, -176, 2216, 0);
	  moveWay(719, 665, 678, 0);
	  moveWay(601, 561, 571, 1);
	  moveWay(-1320, -1226, -1249, 1);
	  moveWay(4900, -2834, 418, 1);
	  moveWay(426, 863, 662, 1);
	  moveWay(388, 710, 565, 1);
	  moveWay(-814, -1573, -1227, 0);

	  // move 5: piece=2 source_mm=(59.7, 153.2) dest_mm=(92.5, 98.4)
	  moveWay(0, 0, 0, 0);
	  moveWay(-4594, 1049, 1688, 0);
	  moveWay(699, 798, 536, 0);
	  moveWay(587, 658, 468, 1);
	  moveWay(-1286, -1456, -1005, 1);
	  moveWay(4494, 294, -3335, 1);
	  moveWay(427, 777, 771, 1);
	  moveWay(388, 647, 643, 1);
	  moveWay(-815, -1423, -1414, 0);

	  // move 6: piece=4 source_mm=(152.5, 62.5) dest_mm=(149.0, 94.2)
	  moveWay(0, 0, 0, 0);
	  moveWay(-6738, 2935, 2666, 0);
	  moveWay(862, 570, 583, 0);
	  moveWay(704, 494, 503, 1);
	  moveWay(-1566, -1064, -1086, 1);
	  moveWay(6714, -1565, -3835, 1);
	  moveWay(430, 685, 848, 1);
	  moveWay(391, 581, 698, 1);
	  moveWay(-820, -1266, -1547, 0);

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
  while(TRUE)
    {
      termDoWork();//lauscht an der UART Verbindung
    }

#endif

  // Never leave main
  for (;;){}
  return 0;
}

