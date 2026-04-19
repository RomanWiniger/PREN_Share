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

#include "motor_config.h"
#include "platform.h"
#include "ftm0.h"
#include "wait.h"
#include "calculation.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "globals.h"
#include "sensor_config.h"
#include "reserve_pin_config.h"

void resetMoveTimers(void);
void resetRotTimers(void);
void initGlobalsMove(void);
void initGlobalsRot(void);

int32_t setTimerValues(int32_t,int32_t,int32_t);


void motorInit(void){

	//////////////////////////////////////////////////////
	// Timer Initialisation
	//////////////////////////////////////////////////////

	ftm0Init(true,0xFFFF); 	//Includes Clockgating

	// Timers: All timers are Channels of FTM0
	// Set all Timer Channels to OutputCompare in order to use the Channel-Interrupt
	// Disable Outputs: These are Muxed as GPIO in order to Handle Overflows
	FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);
	FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);
	FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0);

	#if ENABLE_ROT
		FTM0->CONTROLS[MOTORROT_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0) ;
	#endif


#if ENABLE_STEP

	MOTOR1_STEP_MUX_GPIO();
	MOTOR2_STEP_MUX_GPIO();
	MOTOR3_STEP_MUX_GPIO();
	#if ENABLE_ROT
		MOTORROT_STEP_FTM();
	#endif
	MOTOR1_STEP_SET_OUTPUT();
	MOTOR2_STEP_SET_OUTPUT();
	MOTOR3_STEP_SET_OUTPUT();
#if ENABLE_ROT
	MOTORROT_STEP_GPIO_OUTPUT();
#endif

#endif

#if ENABLE_DIR
	MOTOR1_DIR_MUX_GPIO();
	MOTOR2_DIR_MUX_GPIO();
	MOTOR3_DIR_MUX_GPIO();
	#if ENABLE_ROT
		MOTORROT_DIR_MUX_GPIO();
		MOTORROT_DIR_SET_OUTPUT();
	#endif
	MOTOR1_DIR_SET_OUTPUT();
	MOTOR2_DIR_SET_OUTPUT();
	MOTOR3_DIR_SET_OUTPUT();

#endif

#if ENABLE_EN
	MOTOR1_EN_MUX_GPIO(); MOTOR1_EN_DISABLE();
	MOTOR2_EN_MUX_GPIO(); MOTOR2_EN_DISABLE();
	MOTOR3_EN_MUX_GPIO(); MOTOR3_EN_DISABLE();
	#if ENABLE_ROT
		MOTORROT_EN_MUX_GPIO(); MOTORROT_EN_DISABLE();
		MOTORROT_EN_SET_OUTPUT();
	#endif
	MOTOR1_EN_SET_OUTPUT();
	MOTOR2_EN_SET_OUTPUT();
	MOTOR3_EN_SET_OUTPUT();
#endif

}

void moveToInitPos(uint32_t toggle_US){
	bool m1_run = true; 	// Debouce
	bool m2_run = true; 	// Debouce
	bool m3_run = true; 	// Debouce

	while(true){
		waitUs(toggle_US);

		// Check any Sensor  (set false if Motor is at init-Position
		if(SENSOR1_STATUS()){m1_run = false;}
		if(SENSOR2_STATUS()){m2_run = false;}
		if(SENSOR3_STATUS()){m3_run = false;}

		// Step each (allowed) Motor
		if(m1_run){MOTOR1_STEP_GPIO_TOGGLE();}
		if(m2_run){MOTOR2_STEP_GPIO_TOGGLE();}
		if(m3_run){MOTOR3_STEP_GPIO_TOGGLE();}

		if ((!m1_run)&&(!m2_run)&&(!m3_run)){break;}
	}
}

int moveWay(int32_t mot1, int32_t mot2,int32_t mot3){

	uint32_t mot1_Abs = 0;
	uint32_t mot2_Abs = 0;
	uint32_t mot3_Abs = 0;
	bool mot1_Dir = FWD;
	bool mot2_Dir = FWD;
	bool mot3_Dir = FWD;
	int32_t mostMotor = 0;
	int32_t tmp_steps = 0;
	int16_t ramp_mode = 0;

	//////////////////////////////////////////////////////////////////
	///  Init Ramp Mode
	//////////////////////////////////////////////////////////////////
#if RAMP_MODE_TWOSTEP
	Ramp_twostep = true;
#endif
#if RAMP_MODE_PREMIUM
	Ramping_Premium = true;
#endif

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
		MOTOR1_EN_ENABLE();
		MOTOR2_EN_ENABLE();
		MOTOR3_EN_ENABLE();

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
		for(volatile int i = 0; i < 1000; i++){
		    MOTOR1_EN_ENABLE();
		}
	// Minimal Time between setting of DIR and Setting STEP
	waitMs(DIR_MIN_DELAY_TIME_MS);
	#endif

	//////////////////////////////////////////////////////////////////
	///  SET TIMER VALUES
	//////////////////////////////////////////////////////////////////

	mostMotor = calcPulsePause(mot1_Abs,mot2_Abs,mot3_Abs);		//Set timer Values (Global)
	ftm0StopClk();
	FTM0->CNT=0;	// Reset the FTM0-Counter in order to not go go over an unintentional Overflos

	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	if(mot1_Abs!=0){FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot2_Abs!=0){FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot3_Abs!=0){FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}


	//////////////////////////////////////////////////////////////////
	///  START TIMERS
	//////////////////////////////////////////////////////////////////
	RES2_GPIO_HIGH(); // Monitoring
#if RAMP_ACTIVE && RAMP_MODE_PS
	ftm0StartClk(CLK_SRC_GLOBAL,(PS_GLOBAL+4));
#else
	ftm0StartClk((CLK_SRC_GLOBAL),PS_GLOBAL);
#endif
	ftm0EnableIRQ();
	RES2_GPIO_LOW(); // Monitoring ISR-Time



	//////////////////////////////////////////////////////////////////
	///  CHECK STEPS (
	//////////////////////////////////////////////////////////////////

	// All Counters must reach Max Steps, and Innterrupts must be disabled
	// Interrupt disabeling is done in the ISR in the corresponding Channel

	while (true){
		if((Motor1_Step_Curr>=mot1_Abs)&&(Motor2_Step_Curr>=mot2_Abs)&&(Motor3_Step_Curr>=mot3_Abs)){
			if(		((FTM0->CONTROLS[1].CnSC & FTM_CnSC_CHIE(1))==0)&&
					((FTM0->CONTROLS[2].CnSC & FTM_CnSC_CHIE(1))==0)&&
					((FTM0->CONTROLS[4].CnSC & FTM_CnSC_CHIE(1))==0)){
				break;}
		}
#if RAMP_MODE_PS
		if (ramp_mode < 4){
			if(mostMotor ==1){tmp_steps = Motor1_Step_Curr;}
			if(mostMotor ==2){tmp_steps = Motor2_Step_Curr;}
			if(mostMotor ==3){tmp_steps = Motor3_Step_Curr;}

			if (ramp_mode ==0){
				if (tmp_steps >=(RAMP_NUMB_STEPS/RAMP_DIV1)){
					RES2_GPIO_HIGH();
					//FTM0->SC = FTM_SC_CLKS(CLK_SRC_GLOBAL) |  FTM_SC_PS((PS_GLOBAL+3));
					ftm0ChangePS(CLK_SRC_GLOBAL,(PS_GLOBAL+3));
					RES2_GPIO_LOW();
					ramp_mode++;
				}
				;
			}else if(ramp_mode ==1){
				if (tmp_steps >=(RAMP_NUMB_STEPS/RAMP_DIV2)){
					RES2_GPIO_HIGH();
					//FTM0->SC = FTM_SC_CLKS(CLK_SRC_GLOBAL) |  FTM_SC_PS((PS_GLOBAL+2));
					ftm0ChangePS(CLK_SRC_GLOBAL,(PS_GLOBAL+2));
					RES2_GPIO_LOW();
					ramp_mode++;
				}
				;
			}else if(ramp_mode ==2){
				if (tmp_steps >=(RAMP_NUMB_STEPS/RAMP_DIV3)){
					RES2_GPIO_HIGH();
					//FTM0->SC = FTM_SC_CLKS(CLK_SRC_GLOBAL) |  FTM_SC_PS((PS_GLOBAL+1));
					ftm0ChangePS(CLK_SRC_GLOBAL, (PS_GLOBAL+1));
					RES2_GPIO_LOW();
					ramp_mode++;
				}
				;
			}else if(ramp_mode ==3){
				if (tmp_steps >=(RAMP_NUMB_STEPS)){
					RES2_GPIO_HIGH();
					ftm0ChangePS(CLK_SRC_GLOBAL,PS_GLOBAL);
					//FTM0->SC = FTM_SC_CLKS(CLK_SRC_GLOBAL) |  FTM_SC_PS((PS_GLOBAL));
					RES2_GPIO_LOW();
					ramp_mode++;
				}
			}
		}
#elif RAMP_MODE_NSTEP
		if(mostMotor ==1){tmp_steps = Motor1_Step_Curr;}
		if(mostMotor ==2){tmp_steps = Motor2_Step_Curr;}
		if(mostMotor ==3){tmp_steps = Motor3_Step_Curr;}
		if(tmp_steps>=RAMP_NUMB_STEPS){Ramp_twostep=false;}

#elif RAMP_MODE_PREMIUM
		if(mostMotor ==1){tmp_steps = Motor1_Step_Curr;}
		if(mostMotor ==2){tmp_steps = Motor2_Step_Curr;}
		if(mostMotor ==3){tmp_steps = Motor3_Step_Curr;}
		if(tmp_steps>=RAMP_NUMB_STEPS){Ramping_Premium=false;}
#endif
	}
	initGlobalsMove();

	//////////////////////////////////////////////////////////////////
	///  END ACTION
	//////////////////////////////////////////////////////////////////
	// No need to reset the timer Chanels: CHIE ist deactivated in the corresponding ISR (see FTM0.c)

	waitMs(100*MOTOR_PULSE_US/1000);	// Wait 100 Pulse-width for last Pulse to finish

	resetMoveTimers();					// RESET Global Variables
	ftm0StopClk();						// Stop CLK and Prescaler (All Channels)
	return 1;							// SUCCESS

			}



int moveRotation(uint32_t RotSteps){

#if !ENABLE_ROT
    return 0;  // früh raus wenn ROT deaktiviert
#endif
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
		FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;
	}
	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	if(Steps_Abs!=0){FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}

	// Mux Pins to FTM-Channel
	MOTORROT_STEP_MUX_FTM();


	ftm0StartClk(CLK_SRC_GLOBAL,PS_GLOBAL);
	ftm0EnableIRQ();

	//Wait forever until Steps are reachend
	while (true){
		if(MotorRot_Step_Curr>=Steps_Abs){break;}
		initGlobalsRot();
	}

	waitMs((MOTOR_PULSE_US/1000)*1000);	// Wait 100 Pulse-width for last Pulse to finish
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


void initGlobalsMove(void){
	Motor1_Step_OF =0;
	Motor2_Step_OF =0;
	Motor3_Step_OF =0;
	Motor1_Step_OF_Curr =0;
	Motor2_Step_OF_Curr =0;
	Motor3_Step_OF_Curr =0;

	for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) { //
	    Motor1_Step_Corrector[i] = 0;
	}
	for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) { //
	    Motor2_Step_Corrector[i] = 0;
	}
	for (int i = 0; i < NUM_CORRECTOR_LOOPS; i++) { //
	    Motor3_Step_Corrector[i] = 0;
	}
}

void initGlobalsRot(void){
	MotorRot_Step_OF =0;
	MotorRot_Step_OF_Curr =0;
}
