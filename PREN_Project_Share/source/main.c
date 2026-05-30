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
	  // move 1: piece=1 source_mm=(65.6, 146.1) dest_mm=(34.7, 29.6)
	  moveWay(2926, 2484, 2451, 0, 0);
	  moveWay(354, -1600, 1771, 0, 1);
	  moveWay(714, 783, 539, 1, 0);
	  moveWay(598, 647, 471, 1, 0);
	  moveWay(-1312, -1431, -1010, 1, 0);
	  moveWay(2854, -756, -1727, 1, 1);
	  moveWay(978, 1525, 1219, 1, 0);
	  moveWay(-978, -1525, -1219, 0, 0);

	  // move 2: piece=2 source_mm=(147.9, 148.4) dest_mm=(95.7, 33.4)
	  moveWay(0, 0, 0, 0, 0);
	  moveWay(-3041, 2301, -185, 0, 1);
	  moveWay(726, 667, 668, 1, 0);
	  moveWay(605, 563, 564, 1, 0);
	  moveWay(-1331, -1231, -1232, 1, 0);
	  moveWay(3046, -1049, -1288, 1, 1);
	  moveWay(974, 1369, 1399, 1, 0);
	  moveWay(-974, -1369, -1399, 0, 0);

	  // move 3: piece=4 source_mm=(156.8, 55.7) dest_mm=(158.7, 31.2)
	  moveWay(0, 0, 0, 0, 0);
	  moveWay(-5018, 2741, 2633, 0, 1);
	  moveWay(869, 557, 581, 1, 0);
	  moveWay(710, 485, 502, 1, 0);
	  moveWay(-1579, -1042, -1083, 1, 0);
	  moveWay(5108, -1197, -3885, 1, 1);
	  moveWay(970, 1183, 1554, 1, 0);
	  moveWay(-970, -1183, -1554, 0, 0);

	  // move 4: piece=0 source_mm=(246.0, 146.6) dest_mm=(29.2, 94.3)
	  moveWay(0, 0, 0, 0, 0);
	  moveWay(-2835, 1794, 735, 0, 1);
	  moveWay(706, 517, 802, 1, 0);
	  moveWay(593, 455, 662, 1, 0);
	  moveWay(-1299, -972, -1464, 1, 0);
	  moveWay(4532, -4943, 1893, 1, 1);
	  moveWay(824, 1559, 1246, 1, 0);
	  moveWay(-824, -1559, -1246, 0, 0);

	  // move 5: piece=5 source_mm=(247.1, 53.6) dest_mm=(90.7, 98.3)
	  moveWay(0, 0, 0, 0, 0);
	  moveWay(-6477, 6297, -246, 0, 1);
	  moveWay(843, 448, 688, 1, 0);
	  moveWay(693, 404, 582, 1, 0);
	  moveWay(-1536, -853, -1271, 1, 0);
	  moveWay(6472, -5044, -1253, 1, 1);
	  moveWay(819, 1417, 1419, 1, 0);
	  moveWay(-819, -1417, -1419, 0, 0);

	  // move 6: piece=3 source_mm=(64.0, 57.4) dest_mm=(154.9, 92.4)
	  moveWay(0, 0, 0, 0, 0);
	  moveWay(-6510, 1395, 4654, 0, 1);
	  moveWay(847, 676, 468, 1, 0);
	  moveWay(695, 573, 419, 1, 0);
	  moveWay(-1542, -1249, -887, 1, 0);
	  moveWay(6475, 183, -5951, 1, 1);
	  moveWay(827, 1236, 1566, 1, 0);
	  moveWay(-827, -1236, -1566, 0, 0);

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

