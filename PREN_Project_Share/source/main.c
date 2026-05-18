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
  moveWay(  400,  3000,  1520,  90);  // 1: asymmetric positive
  moveWay( -400, -3000, -1520, -90);  // 2: reverse of 1
  moveWay(  400,  1520,  3000,  90);  // 1: asymmetric positive
  moveWay( -400, -1520, -3000, -90);  // 2: reverse of 1
  moveWay( 3000,  2500,  1000,  45);  // 3: large positive
  moveWay(-3000, -2500, -1000, -45);  // 4: large negative
  moveWay(  100,    50,   200,   0);  // 5: small steps, no rotation
  moveWay( 2000, -1500,  1200,  90);  // 6: mixed directions
  moveWay(-2500,  3000,  -800, -90);  // 7: mixed directions
  moveWay(    0,  2000,  2000,  45);  // 8: motor 1 idle
  moveWay( 1500,     0,  1500,   0);  // 9: motor 2 idle
  moveWay( 3000,  3000,  3000,  90);  // 10: all at max
  moveWay(682, -1668, 1300, 0);
  moveWay(683, -1663, 1302, 0);
  moveWay(-1668, 682, 1300, 0);
  moveWay(-1668, 1300, 682, 0);
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

  moveToInitPos(1000);
  moveWay(3019, 2441, 2418, 0 );
  moveWay(682, -1668, 1300, 0);
  while(TRUE)
    {
      termDoWork();//lauscht an der UART Verbindung
    }

#endif

  // Never leave main
  for (;;){}
  return 0;
}

