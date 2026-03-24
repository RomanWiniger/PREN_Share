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
#include <limits.h>

void resetMoveTimers(void);
void resetRotTimers(void);

int32_t setTimerValues(int32_t,int32_t,int32_t);


void motorInit(void){

	//////////////////////////////////////////////////////
	// Timer Initialisation
	//////////////////////////////////////////////////////

	ftm0Init(true,0xFFFF); 	//Includes Clockgating

	// Timers: All timers are Channels of FTM0
	// Set all Timer Channels to OutputCompare in order to use the Channel-Interrupt
	// Disable Outputs: These are Muxed as GPIO in order to Handle Overflows
	FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);
	FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);
	FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);

	#if ENABLE_ROT
		FTM0->CONTROLS[MOTORROT_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0) ;
	#endif

	//////////////////////////////////////////////////////
	// Pin Muxing / Clockgateing GPIO
	//////////////////////////////////////////////////////

	 SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;

#if ENABLE_STEP

	MOTOR1_STEP_GPIO();
	MOTOR2_STEP_GPIO();
	MOTOR3_STEP_GPIO();
	#if ENABLE_ROT
		MOTORROT_STEP_FTM();
	#endif
	MOTOR1_STEP_GPIO_OUTPUT();
	MOTOR2_STEP_GPIO_OUTPUT();
	MOTOR3_STEP_GPIO_OUTPUT();
#if ENABLE_ROT
	MOTORROT_STEP_GPIO_OUTPUT();
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

	Motor1_Step_Max = mot1_Abs;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor2_Step_Max = mot2_Abs;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM
	Motor3_Step_Max = mot3_Abs;		// Set Stepper-Numer to Global, For Accessibility ISR of FTM


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

	mostMotor = setTimerValues(mot1_Abs,mot2_Abs,mot3_Abs);		//Set timer Values (Global)



	// Set Initial Pause Value
	if(mot1_Abs !=0){
		if(mostMotor==1){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Motor1_Pause/2);}
	}
	if(mot2_Abs !=0){
		if(mostMotor==2){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Motor2_Pause/2);}
	}
	if(mot3_Abs!=0){
		if(mostMotor==3){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Motor3_Pause/2);}
	}

	FTM0->CNT=0;	// Reset the FTM0-Counter in order to not go go over an unintentional Overflos

	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	if(mot1_Abs!=0){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot2_Abs!=0){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot3_Abs!=0){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}


	//////////////////////////////////////////////////////////////////
	///  START TIMERS
	//////////////////////////////////////////////////////////////////

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

	waitMs((MOTOR_PULSE_NS/1000)*100);	// Wait 100 Pulse-width for last Pulse to finish
	FTM0->MOD = 0x0000;
	resetMoveTimers();					// RESET Global Variables
	ftm0StopClk();						// Stop CLK and Prescaler (All Channels)
	return 1;							// SUCCESS
}

int moveRotation(uint32_t RotSteps){

	uint32_t Steps_Abs = 0;
	bool Dir = FWD;

	//////////////////////////////////////////////////////////////////
	///  GET DIRECTION
	//////////////////////////////////////////////////////////////////

	if (RotSteps>=0){
		Steps_Abs = (uint32_t)RotSteps;
		Dir = FWD;
	}else{
		Steps_Abs = (uint32_t)(-RotSteps);
		Dir = REV;
	}

	//////////////////////////////////////////////////////////////////
	///  SET ENABLE
	//////////////////////////////////////////////////////////////////

	// Has to be a minimal Time in advance of the Direction Setting
	#if ENABLE_EN
		if (Steps_Abs>0){MOTORROT_EN_ENABLE();}else{MOTORROT_EN_DISABLE();}

	// Minimal Time between setting of ENABLE and Setting DIR
	waitMs(ENABLE_MIN_DELAY_TIME_MS);
	#endif

	//////////////////////////////////////////////////////////////////
	///  SET DIRECTION
	//////////////////////////////////////////////////////////////////

	#if ENABLE_DIR
		if(Dir){MOTORROT_DIR_FWD();}else{MOTORROT_DIR_REV();}

	// Minimal Time between setting of DIR and Setting STEP
	waitMs(DIR_MIN_DELAY_TIME_MS);
	#endif

	//////////////////////////////////////////////////////////////////
	///  SET TIMER VALUES
	//////////////////////////////////////////////////////////////////

	// Set Initial Pause Value
	if(Steps_Abs !=0){
		FTM0->CONTROLS[MOTORROT_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;
	}
	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	if(Steps_Abs!=0){FTM0->CONTROLS[MOTORROT_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}

	// Mux Pins to FTM-Channel
	MOTORROT_STEP_FTM();


	ftm0StartClk(CLK_SRC_GLOBAL,PS_GLOBAL);
	ftm0StartIRQ();

	//Wait forever until Steps are reachend
	while (true){
		if(MotorRot_Step_Curr>=Steps_Abs){break;}
	}

	waitMs((MOTOR_PULSE_NS/1000)*100);	// Wait 100 Pulse-width for last Pulse to finish
	resetRotTimers();				// RESET Global Variables
	ftm0StopClk();				// Stop CLK and Prescaler (All Channels)return 1;
	return 1;

}

void resetMoveTimers(void){

	ftm0StopIRQ();
	Motor1_Step_Curr =0;
	Motor2_Step_Curr =0;
	Motor3_Step_Curr =0;

	Motor1_Step_Max=0;
	Motor2_Step_Max=0;
	Motor3_Step_Max=0;
}

void resetRotTimers(void){

	ftm0StopIRQ();
	MotorRot_Step_Curr =0;
	MotorRot_Step_Max=0;
}


int32_t setTimerValues(int32_t Mot1,int32_t Mot2, int32_t Mot3){
	int32_t mostMotor;
	int32_t mostSteps=0;
	int64_t totalTicks;
	int64_t totalTicksPauseM1;
	int64_t totalTicksPauseM2;
	int64_t totalTicksPauseM3;
	int errorStepperM1;
	int errorStepperM2;
	int errorStepperM3;
	int64_t tmp_M1_Pause;
	int64_t tmp_M2_Pause;
	int64_t tmp_M3_Pause;



	// Check which motor has to do the most steps:
		if (Mot1>=Mot2 && Mot1>=Mot3){
			mostSteps = Mot1;
			mostMotor=1;
			tmp_M1_Pause = MOTOR_MINPAUSE_MOD_TICK;
			for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) {
			    Motor1_Step_Corrector[i] = 0;
			}
			totalTicks=((int64_t)tmp_M1_Pause +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot1;

		}else{
			if (Mot2>=Mot1 && Mot2>=Mot3){
				mostSteps = Mot2;
				mostMotor=2;
				tmp_M2_Pause = MOTOR_MINPAUSE_MOD_TICK;
				for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) {
							    Motor2_Step_Corrector[i] = 0;
							}
				totalTicks=((int64_t)MOTOR_MINPAUSE_MOD_TICK +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot2;
			}else{
				if (Mot3>=Mot2 && Mot3>=Mot1){
						mostSteps = Mot3;
						mostMotor=3;
						tmp_M3_Pause = MOTOR_MINPAUSE_MOD_TICK;
						for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) {
						    Motor3_Step_Corrector[i] = 0;
						}
						totalTicks=((int64_t)MOTOR_MINPAUSE_MOD_TICK +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot3;
				}else{
					return 0;
				}
			}
		}

		//Time Distance between Pulses for Each Motor
		if (mostMotor != 1 && Mot1!=0){
			totalTicksPauseM1 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot1);
			tmp_M1_Pause = totalTicksPauseM1 / Mot1;
			errorStepperM1 = totalTicks - ((tmp_M1_Pause +MOTOR_PULSE_MOD_TICK)*Mot1);
			Motor1_Step_Corrector[0] = (Mot1/errorStepperM1)+1;				// Round up to omit overshoot
			for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
				errorStepperM1= errorStepperM1 -(1/Motor1_Step_Corrector[i])*Mot1;	// second correction
				Motor1_Step_Corrector[i+1]=Mot1/errorStepperM1;
			}
		}
		if(Mot1==0){tmp_M1_Pause =0;}

		if (mostMotor != 2 && Mot2!=0){
			totalTicksPauseM2 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot2);
			tmp_M2_Pause = totalTicksPauseM2 / Mot2;
			errorStepperM2 = totalTicks - ((tmp_M2_Pause +MOTOR_PULSE_MOD_TICK)*Mot2);
			Motor2_Step_Corrector[0] = (Mot2/errorStepperM2)+1;				// Round up to omit overshoot
			for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
				errorStepperM2= errorStepperM2 -(1/Motor2_Step_Corrector[i])*Mot2;	// second correction
				Motor2_Step_Corrector[i+1]=Mot2/errorStepperM2;
			}
		}
		if(Mot2==0){tmp_M2_Pause =0;}

		if (mostMotor != 3 && Mot3!=0){
			totalTicksPauseM3 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot3);
			tmp_M3_Pause = totalTicksPauseM3 / Mot3;
			errorStepperM3 = totalTicks - ((tmp_M3_Pause +MOTOR_PULSE_MOD_TICK)*Mot3);
			Motor3_Step_Corrector[0] = (Mot3/errorStepperM3)+1;				// Round up to omit overshoot
			for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
				errorStepperM3= errorStepperM3 -(1/Motor3_Step_Corrector[i])*Mot3;	// second correction
				Motor3_Step_Corrector[i+1]=Mot3/errorStepperM3;
			}
		}
		if(Mot3==0){tmp_M3_Pause =0;}

#if OVERFLOW_HANDLING
		do{		// If Modulo Overflow: Count how many INT16_MAX Overflows occur
			if (tmp_M1_Pause>=INT16_MAX){
				tmp_M1_Pause=(tmp_M1_Pause-INT16_MAX);
				Motor1_Step_OF +=1;
			}
			if (tmp_M2_Pause>=INT16_MAX){
				tmp_M2_Pause=(tmp_M2_Pause-INT16_MAX);
				Motor2_Step_OF +=1;
			}
			if (tmp_M3_Pause>=INT16_MAX){
				tmp_M3_Pause=(tmp_M3_Pause-INT16_MAX);
				Motor3_Step_OF +=1;
			}
		}while((tmp_M1_Pause>=INT16_MAX || tmp_M2_Pause>=INT16_MAX||tmp_M3_Pause>=INT16_MAX));
#else
		if((tmp_M1_Pause>=INT16_MAX || tmp_M2_Pause>=INT16_MAX||tmp_M3_Pause>=INT16_MAX)){
			printf("error!");
		};
#endif

		Motor1_Pause = (uint16_t)tmp_M1_Pause;
		Motor2_Pause = (uint16_t)tmp_M2_Pause;
		Motor3_Pause = (uint16_t)tmp_M3_Pause;

		return mostMotor;
}

