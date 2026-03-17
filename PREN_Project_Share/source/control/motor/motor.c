/**
 *--------------------------------------------------------------------\n
 *          PREN2 Group 1, Stepper Motors          \n
 *--------------------------------------------------------------------\n
 *
 * \brief         stepper motor controller
 * \file
 * \author        Roman WIniger
 * \date          27.02.2026
 *
 *--------------------------------------------------------------------
**/

#include <motor_config.h>
#include "platform.h"
#include "ftm0.h"
#include "globals.h"
#include "wait.h"
#include <stdint.h>
#include <stdbool.h>

void resetTimers(void);
int32_t setTimerValues(int32_t,int32_t,int32_t);


void motorInit(void){

	//////////////////////////////////////////////////////
	// Timer Initialisation
	//////////////////////////////////////////////////////

	ftm0Init(true,0xFFFF); 	//Includes Clockgating

	// Timers: All timers are Channels of FTM0
	// Set all Timer Channels to OutputCompare
	// Set all Timer Channels to Set High on Match
	FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
	FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
	FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
#if ENABLE_ROT
	FTM0->CONTROLS[MOTORROT_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
#endif

	//////////////////////////////////////////////////////
	// Pin Muxing / Clockgateing GPIO
	//////////////////////////////////////////////////////

	 SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
	  //SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
	  //SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;

#if ENABLE_STEP

	MOTOR1_STEP_GPIO();
	MOTOR2_STEP_GPIO();
	MOTOR3_STEP_GPIO();
	#if ENABLE_ROT
		MOTORROT_STEP_FTM();
	#endif

#endif

#if ENABLE_DIR
	MOTOR1_DIR_GPIO();
	MOTOR2_DIR_GPIO();
	MOTOR3_DIR_GPIO();
	#if ENABLE_ROT
		MOTORROT_DIR_GPIO();
	#endif
	MOTOR_DIR_OUTPUT();

#endif

#if ENABLE_EN
	MOTOR1_EN_GPIO();
	MOTOR2_EN_GPIO();
	MOTOR3_EN_GPIO();
	#if ENABLE_ROT
		MOTORROT_EN_GPIO();
		MOTORROT_EN_OUTPUT();
	#endif
		MOTOR_EN_OUTPUT();
#endif


}

int moveWay(int32_t mot1, int32_t mot2,int32_t mot3){

	int32_t mostMotor;
	uint32_t mot1_Abs = 0;
	uint32_t mot2_Abs = 0;
	uint32_t mot3_Abs = 0;
	bool mot1_Dir = FWD;
	bool mot2_Dir = FWD;
	bool mot3_Dir = FWD;
	Motor1_Step_Max = mot1;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor2_Step_Max = mot2;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor3_Step_Max = mot3;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM

	//////////////////////////////////////////////////////////////////
	///  GET DIRECTION
	//////////////////////////////////////////////////////////////////

	// Abs-Value of Steps
	if (mot1>=0){
		mot1_Abs = (uint32_t)mot1;
		mot1_Dir = FWD;
	}else{
		mot1_Abs = (uint32_t)(-mot1);
		mot1_Dir = REV;
	}

	if (mot2>=0){
		mot2_Abs = (uint32_t)mot2;
		mot2_Dir = FWD;
	}else{
		mot2_Abs = (uint32_t)(-mot2);
		mot2_Dir = REV;
	}

	if (mot3>=0){
		mot3_Abs = (uint32_t)mot3;
		mot3_Dir = FWD;
	}else{
		mot3_Abs = (uint32_t)(-mot3);
		mot3_Dir = REV;
	}

	//////////////////////////////////////////////////////////////////
	///  SET ENABLE
	//////////////////////////////////////////////////////////////////

	// Has to be a minimal Time in advance of the Direction Setting
	#if ENABLE_EN
		if (mot1_Abs>0){MOTOR1_EN_ENABLE();}else{MOTOR1_EN_DISABLE();}
		if (mot2_Abs>0){MOTOR2_EN_ENABLE();}else{MOTOR2_EN_DISABLE();}
		if (mot3_Abs>0){MOTOR3_EN_ENABLE();}else{MOTOR3_EN_DISABLE();}

	// Minimal Time between setting of ENABLE and Setting DIR
	waitMs(ENABLE_MIN_DELAY_TIME_MS);
	#endif

	//////////////////////////////////////////////////////////////////
	///  SET DIRECTION
	//////////////////////////////////////////////////////////////////

	#if ENABLE_DIR
		if(mot1_Dir){MOTOR1_DIR_FWD();}else{MOTOR1_DIR_REV();}
		if(mot2_Dir){MOTOR2_DIR_FWD();}else{MOTOR2_DIR_REV();}
		if(mot3_Dir){MOTOR3_DIR_FWD();}else{MOTOR3_DIR_REV();}

	// Minimal Time between setting of DIR and Setting STEP
	waitMs(DIR_MIN_DELAY_TIME_MS);
	#endif

	//////////////////////////////////////////////////////////////////
	///  SET TIMER VALUES
	//////////////////////////////////////////////////////////////////

	mostMotor = setTimerValues(mot1,mot2,mot3);		//Set timer Values (Global)

	// Set Initial Pause Value
	if(mot1_Abs !=0){
		if(mostMotor==1){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+Motor1_Pause/2;}
	}
	if(mot2_Abs !=0){
		if(mostMotor==2){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+Motor2_Pause/2;}
	}
	if(mot3_Abs!=0){
		if(mostMotor==3){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+Motor3_Pause/2;}
	}

	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	if(mot1_Abs!=0){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot2_Abs!=0){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot3_Abs!=0){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}



	//////////////////////////////////////////////////////////////////
	///  SET STEPS (TIMERS)
	//////////////////////////////////////////////////////////////////

	// Mux Pins to FTM-Channel
	MOTOR1_STEP_FTM();
	MOTOR2_STEP_FTM();
	MOTOR3_STEP_FTM();

	ftm0StartClk(CLK_SRC_GLOBAL,PS_GLOBAL);
	ftm0StartIRQ();


	//////////////////////////////////////////////////////////////////
	///  CHECK STEPS
	//////////////////////////////////////////////////////////////////
	while (true){
		if((Motor1_Step_Curr>=mot1_Abs)&&(Motor2_Step_Curr>=mot2_Abs)&&(Motor3_Step_Curr>=mot3_Abs)){break;}
	}

	//////////////////////////////////////////////////////////////////
	///  END ACTION
	//////////////////////////////////////////////////////////////////
	// No need to reset the timer Chanels: CHIE ist deactivated in the corresponding ISR (see FTM0.c)

	waitMs(MOTOR_PULSE_MS*10);	// Wait 10 Pulse-width for last Pulse to finish
	resetTimers();				// RESET Global Variables

	return 1;					// SUCCESS
}

int moveRotation(uint32_t RotSteps){
	return 1;
	//Todo
}

void resetTimers(void){

	ftm0StopIRQ();
	Motor1_Step_Curr =0;
	Motor2_Step_Curr =0;
	Motor3_Step_Curr =0;

	Motor1_Step_Max=0;
	Motor2_Step_Max=0;
	Motor3_Step_Max=0;
}


int32_t setTimerValues(int32_t Mot1,int32_t Mot2, int32_t Mot3){
	int32_t mostMotor;
	int32_t mostSteps=0;
	double movingTime_MS=0;
	double mot1_Dist_MS=0;
	double mot2_Dist_MS=0;
	double mot3_Dist_MS=0;


	// Check which motor has to do the most steps:
		if (Mot1>=Mot2 && Mot1>=Mot3){
			mostSteps = Mot1;
			mostMotor=1;
		}else{
			if (Mot2>=Mot1 && Mot2>=Mot3){
				mostSteps = Mot2;
				mostMotor=2;
			}else{
				if (Mot3>=Mot2 && Mot3>=Mot1){
						mostSteps = Mot3;
						mostMotor=3;
				}else{
					return 0;
				}
			}
		}

		//Calculate Way Timelength in ms
		movingTime_MS = ((mostSteps-1)*MIN_STEP_DISTANCE_MS);

		//Time Distance between Pulses for Each Motor
		if (mostMotor == 1){
			Motor1_Pause =(uint16_t)TIMER_CLK_SCAL/1000;
		}else if(Mot1!=0){
			mot1_Dist_MS = (movingTime_MS/(Mot1))-MOTOR_PULSE_MS;
			Motor1_Pause =(uint16_t)((TIMER_CLK_SCAL)*mot1_Dist_MS/1000);
		}else{Motor1_Pause =0;}


		if (mostMotor == 2){
			Motor2_Pause =(uint16_t)TIMER_CLK_SCAL/1000;
		}else if(Mot2!=0){
			mot2_Dist_MS = (movingTime_MS/(Mot2))-MOTOR_PULSE_MS;
			Motor2_Pause =(uint16_t)((TIMER_CLK_SCAL)*mot2_Dist_MS/1000);
		}else{Motor2_Pause =0;}

		if (mostMotor == 3){
			Motor3_Pause =(uint16_t)TIMER_CLK_SCAL/1000;
		}else if(Mot3!=0){
			mot3_Dist_MS = (movingTime_MS/(Mot3))-MOTOR_PULSE_MS;
			Motor3_Pause =(uint16_t)((TIMER_CLK_SCAL)*mot3_Dist_MS/1000);
		}else{Motor3_Pause =0;}

		return mostMotor;
}

