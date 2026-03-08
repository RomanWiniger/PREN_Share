/**
 *--------------------------------------------------------------------\n
 *          PREN2 Gruppe 1, Stepper Motors          \n
 *--------------------------------------------------------------------\n
 *
 * \brief         color utils
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




void resetTimers(void);
int32_t setTimerValues(int32_t Mot1,int32_t Mot2, int32_t Mot3);



void motorInit(void){

	//////////////////////////////////////////////////////
	// Timer Initialisation
	//////////////////////////////////////////////////////

	ftm0Init(true,0xFFFF); 	//Includes Clockgating


	// Set all Timer Channels to OutputCompare
	// Set all Timer Channels to Set High on Match
	FTM0->CONTROLS[MOTOR1_TIMER_CHAN].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
	FTM0->CONTROLS[MOTOR2_TIMER_CHAN].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;
	FTM0->CONTROLS[MOTOR3_TIMER_CHAN].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(1) ;


	//////////////////////////////////////////////////////
	// Pin Mux
	//////////////////////////////////////////////////////
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
#endif

#if ENABLE_EN
	MOTOR1_EN_GPIO();
	MOTOR2_EN_GPIO();
	MOTOR3_EN_GPIO();
	#if ENABLE_ROT
		MOTORROT_EN_GPIO();
	#endif
#endif


}

int moveWay(uint32_t Mot1,bool dir1, uint32_t Mot2,bool dir2,uint32_t Mot3, bool dir3){

	int32_t mostMotor;

#if ENABLE_DIR
	if(dir1){MOTOR1_DIR_FWD();}else{MOTOR1_DIR_REV();}
	if(dir2){MOTOR2_DIR_FWD();}else{MOTOR2_DIR_REV();}
	if(dir3){MOTOR3_DIR_FWD();}else{MOTOR3_DIR_REV();}
#endif


#if ENABLE_DIR
	MOTOR1_EN();
	MOTOR2_EN();
	MOTOR3_EN();
#endif


	Motor1_Step_Max = Mot1;
	Motor2_Step_Max = Mot2;
	Motor3_Step_Max = Mot3;

	mostMotor = setTimerValues(Mot1,Mot2,Mot3);		//Set timer Values (Global)

	// Set Channel Value: 1 = 0.52ms
	if(Mot1 !=0){
		if(mostMotor==1){FTM0->CONTROLS[MOTOR1_TIMER_CHAN].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_TIMER_CHAN].CnV =FIRST_PULSE_START_MOD+Motor1_Pause/2;}
	}
	if(Mot2 !=0){
		if(mostMotor==2){FTM0->CONTROLS[MOTOR2_TIMER_CHAN].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_TIMER_CHAN].CnV =FIRST_PULSE_START_MOD+Motor2_Pause/2;}
	}
	if(Mot3!=0){
		if(mostMotor==3){FTM0->CONTROLS[MOTOR3_TIMER_CHAN].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_TIMER_CHAN].CnV =FIRST_PULSE_START_MOD+Motor3_Pause/2;}
	}
	// activate Timer Interrupts (Channels) unless Stepnumber isnt Zero

	if(Mot1!=0){FTM0->CONTROLS[MOTOR1_TIMER_CHAN].CnSC |= FTM_CnSC_CHIE(1);}
	if(Mot2!=0){FTM0->CONTROLS[MOTOR2_TIMER_CHAN].CnSC |= FTM_CnSC_CHIE(1);}
	if(Mot3!=0){FTM0->CONTROLS[MOTOR3_TIMER_CHAN].CnSC |= FTM_CnSC_CHIE(1);}


	Motor1_Step_Max = Mot1;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor2_Step_Max = Mot2;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor3_Step_Max = Mot3;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM

	MOTOR1_STEP_FTM();
	MOTOR2_STEP_FTM();
	MOTOR3_STEP_FTM();

	ftm0StartClk(CLK_SRC_GLOBAL,PS_GLOBAL);
	ftm0StartIRQ();

	while (true){
		if((Motor1_Step_Curr>=Mot1)&&(Motor2_Step_Curr>=Mot2)&&(Motor3_Step_Curr>=Mot3)){break;}
	}

	waitMs(MOTOR_PULSE_MS*10);	//Wait 10 Pulsewidth for last Pulse to finish
	resetTimers();

	return 1;
}
int moveRotation(uint32_t RotSteps){
	//Todo
};
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

