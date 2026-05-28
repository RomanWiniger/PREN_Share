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
#include "emergency.h"
#include "reserve_pin_config.h"
#include <string.h> // für memset für Array mit true Initialisierung

void resetMoveTimers(void);
void resetRotTimers(void);
void initGlobalsMove(void);
void initGlobalsRot(void);
void moveToInitPos(uint32_t toggle_US);

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
		FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0) ;
	#endif

	emergencyStop_Init();

#if RAMP_MODE_NSTEP
	FTM0->CONTROLS[6].CnSC =  FTM_CnSC_MSB(0) |FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(0) | FTM_CnSC_ELSA(0) ; // Ramp Sequence Incrementer
#endif

#if RAMP_MODE_NSTEP
	memset(Ramp_M1_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));
	memset(Ramp_M2_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));
	memset(Ramp_M3_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));

#endif

#if ENABLE_STEP

	MOTOR1_STEP_MUX_GPIO();
	MOTOR2_STEP_MUX_GPIO();
	MOTOR3_STEP_MUX_GPIO();
	#if ENABLE_ROT
	MOTORROT_STEP_MUX_GPIO();
	#endif
	MOTOR1_STEP_SET_OUTPUT();
	MOTOR2_STEP_SET_OUTPUT();
	MOTOR3_STEP_SET_OUTPUT();
#if ENABLE_ROT
	MOTORROT_STEP_SET_OUTPUT();
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
#if !SIM_SENSORS	// for debugging withour Sensors
	moveToInitPos(100);
#endif
}


void moveToInitPos(uint32_t toggle_US){
    bool m1_phase1 = true;
    bool m2_phase1 = true;
    bool m3_phase1 = true;

    const int32_t BACKOFF_STEPS = 800;

    MOTOR1_EN_ENABLE();
    MOTOR2_EN_ENABLE();
    MOTOR3_EN_ENABLE();

    // === Phase 1: Schnell zum Sensor ===
    MOTOR1_DIR_REV();
    MOTOR2_DIR_REV();
    MOTOR3_DIR_REV();

    while(m1_phase1 || m2_phase1 || m3_phase1){
        waitUs(toggle_US);

        if(!SENSOR1_STATUS()){ m1_phase1 = false; }
        if(!SENSOR2_STATUS()){ m2_phase1 = false; }
        if(!SENSOR3_STATUS()){ m3_phase1 = false; }

        if(m1_phase1){ MOTOR1_STEP_GPIO_TOGGLE(); }
        if(m2_phase1){ MOTOR2_STEP_GPIO_TOGGLE(); }
        if(m3_phase1){ MOTOR3_STEP_GPIO_TOGGLE(); }
    }

    // === Backoff: Alle gleichzeitig weg vom Sensor ===
    MOTOR1_DIR_FWD();
    MOTOR2_DIR_FWD();
    MOTOR3_DIR_FWD();

    for(int32_t i = 0; i < BACKOFF_STEPS; i++){
        waitUs(toggle_US*10);
        MOTOR1_STEP_GPIO_TOGGLE();
        MOTOR2_STEP_GPIO_TOGGLE();
        MOTOR3_STEP_GPIO_TOGGLE();
    }

    // === Phase 2: Langsam zurück zum Sensor ===
    MOTOR1_DIR_REV();
    MOTOR2_DIR_REV();
    MOTOR3_DIR_REV();

    bool m1_phase2 = true;
    bool m2_phase2 = true;
    bool m3_phase2 = true;

    while(m1_phase2 || m2_phase2 || m3_phase2){
        waitUs(toggle_US * 10);

        if(!SENSOR1_STATUS()){ m1_phase2 = false; }
        if(!SENSOR2_STATUS()){ m2_phase2 = false; }
        if(!SENSOR3_STATUS()){ m3_phase2 = false; }

        if(m1_phase2){ MOTOR1_STEP_GPIO_TOGGLE(); }
        if(m2_phase2){ MOTOR2_STEP_GPIO_TOGGLE(); }
        if(m3_phase2){ MOTOR3_STEP_GPIO_TOGGLE(); }
    }
}

int moveWay(int32_t mot1, int32_t mot2,int32_t mot3, int32_t RotSteps){



	uint32_t mot1_Abs = 0;
	uint32_t mot2_Abs = 0;
	uint32_t mot3_Abs = 0;
	uint32_t rot_Abs = 0;

	bool mot1_Dir = FWD;
	bool mot2_Dir = FWD;
	bool mot3_Dir = FWD;
	bool rot_Dir = FWD;

	int32_t mostMotor = 0;
#if RAMP_MODE_END
	int32_t rem_steps = 0;
#endif
	int16_t ramp_mode = 0;

	//////////////////////////////////////////////////////////////////
	///  Init Ramp Mode
	//////////////////////////////////////////////////////////////////


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

    if (RotSteps>=0){
        rot_Abs = (int32_t)RotSteps;
        rot_Dir = FWD;
    }else{
        rot_Abs = (int32_t)(-RotSteps);
        rot_Dir = REV;
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
		MOTORROT_EN_ENABLE();

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
		if(rot_Dir){MOTORROT_DIR_FWD();}else{MOTORROT_DIR_REV();}
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

#if RAMP_MODE_NSTEP
	if (!Ramp_Disabled) {
		FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV = Ramp_M1_End_Rem_Ticks[0];
		FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV = Ramp_M2_End_Rem_Ticks[0];
		FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV = Ramp_M3_End_Rem_Ticks[0];
		FTM0->CONTROLS[6].CnV = Ramp_Step_Ticks[1];
	} else {
		Ramp_Step_Curr = RAMP_NSTEPS + 1;   // ISR springt sofort in Konstant-Pfad

		Motor1_Pause = SLOW_PAUSE_MOD_TICK;
		Motor2_Pause = SLOW_PAUSE_MOD_TICK;
		Motor3_Pause = SLOW_PAUSE_MOD_TICK;

		Ramp_M1_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS] = 0;
		Ramp_M2_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS] = 0;
		Ramp_M3_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS] = 0;

		if(mot1_Abs) FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;
		if(mot2_Abs) FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;
		if(mot3_Abs) FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;
	}
#endif

	FTM0->CNT=0;	// Reset the FTM0-Counter in order to not go go over an unintentional Overflos

	// activate Timer Interrupts (Channels) unless Step number isn't Zero
	FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
#if RAMP_MODE_NSTEP
	if (!Ramp_Disabled) {
	FTM0->CONTROLS[6].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
	}
#endif
	if(mot1_Abs!=0){FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot2_Abs!=0){FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
	if(mot3_Abs!=0){FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);}
#if RAMP_MODE_NSTEP
	if (!Ramp_Disabled) {
	FTM0->CONTROLS[6].CnSC |= FTM_CnSC_CHIE(1); //Ramp Sequence incrementer
	}
#endif

#if DEBUG_MODE_ISR1
	RES1_GPIO_HIGH(); // Monitoring ISR-Time
	RES1_GPIO_LOW(); // Monitoring ISR-Time
#endif

	// FOr Debugging
	FTM0->CONTROLS[7].CnSC;

	if(rot_Abs != 0){

	        MotorRot_Step_Max = rot_Abs;  // FIX 1: war nie gesetzt, ISR vergleicht aber dagegen

	        MotorRot_Pause = MOTOR_MINPAUSE_MOD_TICK/2;

	        // FIX 2: Timer stoppen und Counter resetten – genau wie in moveWay()
	        ftm0StopClk();
	        FTM0->CNT = 0;


	        FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;

	        // FIX 4: Interrupt-Flag clearen BEVOR der Interrupt aktiviert wird
	        FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnSC &= ~FTM_CnSC_CHF(1);
	        FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnSC |= FTM_CnSC_CHIE(1);
	    }


	//////////////////////////////////////////////////////////////////
	///  START TIMERS
	//////////////////////////////////////////////////////////////////
	RES2_GPIO_HIGH(); // Monitoring

	ftm0EnableIRQ();
	ftm0StartClk((CLK_SRC_GLOBAL),PS_GLOBAL);


	RES2_GPIO_LOW(); // Monitoring ISR-Time



	//////////////////////////////////////////////////////////////////
	///  CHECK STEPS (
	//////////////////////////////////////////////////////////////////

	// All Counters must reach Max Steps, and Innterrupts must be disabled
	// Interrupt disabeling is done in the ISR in the corresponding Channel
	#if RAMP_MODE_END
		bool reduce[4] = {true,true,true,true}; // First Pass index
	#endif
	while (true){

	#if RAMP_MODE_END
//		if (ramp_mode < 4){
			if(mostMotor ==1){rem_steps = mot1_Abs-Motor1_Step_Curr;}
			if(mostMotor ==2){rem_steps = mot2_Abs-Motor2_Step_Curr;}
			if(mostMotor ==3){rem_steps = mot3_Abs-Motor3_Step_Curr;}

			if((RAMP_END_PS2 < rem_steps)&&(rem_steps <= RAMP_END_PS1)&&reduce[0]){
				ftm0ReducePS(CLK_SRC_GLOBAL,(PS_GLOBAL+1));
				reduce[0] = false;
			}else if((RAMP_END_PS3 < rem_steps)&&(rem_steps <= RAMP_END_PS2)&&reduce[1]){
				ftm0ReducePS(CLK_SRC_GLOBAL,(PS_GLOBAL+2));
				reduce[1] = false;
			}else if((RAMP_END_PS4 < rem_steps)&&(rem_steps <= RAMP_END_PS3)&&reduce[2]){
				ftm0ReducePS(CLK_SRC_GLOBAL,(PS_GLOBAL+3));
				reduce[2] = false;
			}else if((rem_steps <= RAMP_END_PS4)&&reduce[3]){
				ftm0ReducePS(CLK_SRC_GLOBAL,(PS_GLOBAL+4));
				reduce[3] = false;
			}
//		}
	#endif

		if((Motor1_Step_Curr>=mot1_Abs)&&(Motor2_Step_Curr>=mot2_Abs)&&(Motor3_Step_Curr>=mot3_Abs)&&(MotorRot_Step_Curr >= rot_Abs)){
			if(		((FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnSC & FTM_CnSC_CHIE(1))==0)&&
					((FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnSC & FTM_CnSC_CHIE(1))==0)&&
					((FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnSC & FTM_CnSC_CHIE(1))==0)&&
		            ((FTM0->CONTROLS[MOTORROT_STEP_TIMER_CHNL].CnSC & FTM_CnSC_CHIE(1))==0)){
				break;}
		}
	}
	initGlobalsMove();
	initGlobalsRot();

	//////////////////////////////////////////////////////////////////
	///  END ACTION
	//////////////////////////////////////////////////////////////////
	// No need to reset the timer Chanels: CHIE ist deactivated in the corresponding ISR (see FTM0.c)

	waitMs(100*MOTOR_PULSE_US/1000);	// Wait 100 Pulse-width for last Pulse to finish

	resetMoveTimers();					// RESET Global Variables
	resetRotTimers();
	ftm0StopClk();						// Stop CLK and Prescaler (All Channels)
	return 1;							// SUCCESS

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

#if RAMP_MODE_NSTEP
	memset(Ramp_M1_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));
	memset(Ramp_M2_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));
	memset(Ramp_M3_Rem_Pending, 1, (RAMP_NSTEPS+1) * sizeof(bool));

	memset(Motor1_Step_Corrector, 1, (NUM_CORRECTOR_LOOPS) * sizeof(uint32_t));
	memset(Motor2_Step_Corrector, 1, (NUM_CORRECTOR_LOOPS) * sizeof(uint32_t));
	memset(Motor3_Step_Corrector, 1, (NUM_CORRECTOR_LOOPS) * sizeof(uint32_t));

	Ramp_Step_Curr=0;
#endif

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
