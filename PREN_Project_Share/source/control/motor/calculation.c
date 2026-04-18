/**
 *--------------------------------------------------------------------\n
 *          PREN2 Group 1, Stepper Motors          \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Calculation and Stetting of Globals
 * \file
 * \author        Roman WIniger
 * \date          04.02.2026
 *
 *--------------------------------------------------------------------
**/

#include <motor_config.h>
#include "platform.h"
#include "ftm0.h"
#include "wait.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "globals.h"

static void overflowCounter_16bit(uint64_t number, uint16_t* OF_counter, uint16_t* modulo);
void calcEndPoint(uint64_t temp_ticks, bool startState, bool* endState, int64_t* endTicks);
uint64_t scalePercent(uint64_t value, int32_t percentage);

int32_t calcPulsePause(int32_t Mot1,int32_t Mot2, int32_t Mot3){
	uint32_t mostMotor;
	uint64_t totalTicks;
	uint64_t totalTicksPauseM1;
	uint64_t totalTicksPauseM2;
	uint64_t totalTicksPauseM3;
	uint32_t errorStepperM1;
	uint32_t errorStepperM2;
	uint32_t errorStepperM3;
	uint64_t tmp_M1_Pause;
	uint64_t tmp_M2_Pause;
	uint64_t tmp_M3_Pause;
	uint64_t tmp_M1_Pause_init;		// Tick Value for the FIRST Pause
	uint64_t tmp_M2_Pause_init;		// Tick Value for the FIRST Pause
	uint64_t tmp_M3_Pause_init;		// Tick Value for the FIRST Pause
	uint32_t tmp_M1_OF_CNT=0;	// OverFlow Counter
	uint32_t tmp_M2_OF_CNT=0; // OverFlow Counter
	uint32_t tmp_M3_OF_CNT=0; // OverFlow Counter
#if RAMP_MODE_TWOSTEP
	uint64_t tmp_M1_Ramp_Pause;
	uint64_t tmp_M2_Ramp_Pause;
	uint64_t tmp_M3_Ramp_Pause;
	uint32_t tmp_M1_Ramp_OF_CNT=0; // OverFlow Counter Twostep Ramp
	uint32_t tmp_M2_Ramp_OF_CNT=0; // OverFlow Counter Twostep Ramp
	uint32_t tmp_M3_Ramp_OF_CNT=0; // OverFlow Counter Twostep Ramp
#endif

	//////////////////////////////////////////////////////////////////
	///  FIND MOTOR WITH MOST STEPS
	//////////////////////////////////////////////////////////////////
		if (Mot1>=Mot2 && Mot1>=Mot3){
			mostMotor=1;
			tmp_M1_Pause = MOTOR_MINPAUSE_MOD_TICK;
			totalTicks=((int64_t)tmp_M1_Pause +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot1;

		}else{
			if (Mot2>=Mot1 && Mot2>=Mot3){
				mostMotor=2;
				tmp_M2_Pause = MOTOR_MINPAUSE_MOD_TICK;
				totalTicks=((int64_t)MOTOR_MINPAUSE_MOD_TICK +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot2;
			}else{
				if (Mot3>=Mot2 && Mot3>=Mot1){
						mostMotor=3;
						tmp_M3_Pause = MOTOR_MINPAUSE_MOD_TICK;
						totalTicks=((int64_t)MOTOR_MINPAUSE_MOD_TICK +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot3;
				}else{
					return false; // ERROR
				}
			}
		}

		//////////////////////////////////////////////////////////////////
		///  TICK NUMBER FOR PAUSE BETWENN PULSES
		//////////////////////////////////////////////////////////////////

		if (mostMotor != 1 && Mot1!=0){
			totalTicksPauseM1 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot1);
			tmp_M1_Pause = totalTicksPauseM1 / Mot1;
			errorStepperM1 = totalTicks - ((tmp_M1_Pause +MOTOR_PULSE_MOD_TICK)*Mot1);
			if(errorStepperM1>0){
				Motor1_Step_Corrector[0] = (Mot1/errorStepperM1)+1;				// Round up to omit overshoot
				for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
					if((errorStepperM1>0)&&(Motor1_Step_Corrector[i]>0)){
						errorStepperM1= errorStepperM1 -(1.0/(double)Motor1_Step_Corrector[i])*Mot1;	// second correction
						Motor1_Step_Corrector[i+1]=Mot1/errorStepperM1;
					}
				}
			}
		}
		if(Mot1==0){tmp_M1_Pause =0;}

		if (mostMotor != 2 && Mot2!=0){
			totalTicksPauseM2 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot2);
			tmp_M2_Pause = totalTicksPauseM2 / Mot2;
			errorStepperM2 = totalTicks - ((tmp_M2_Pause +MOTOR_PULSE_MOD_TICK)*Mot2);
			if(errorStepperM2>0){
				Motor2_Step_Corrector[0] = (Mot2/errorStepperM2)+1;				// Round up to omit overshoot
				for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
					if((errorStepperM2>0)&&(Motor2_Step_Corrector[i]>0)){
						errorStepperM2= errorStepperM2 -(1.0/(double)Motor2_Step_Corrector[i])*Mot2;	// second correction
						Motor2_Step_Corrector[i+1]=Mot2/errorStepperM2;
					}
				}
			}
		}
		if(Mot2==0){tmp_M2_Pause =0;}

		if (mostMotor != 3 && Mot3!=0){
			totalTicksPauseM3 = totalTicks - (MOTOR_PULSE_MOD_TICK*Mot3);
			tmp_M3_Pause = totalTicksPauseM3 / Mot3;
			errorStepperM3 = totalTicks - ((tmp_M3_Pause +MOTOR_PULSE_MOD_TICK)*Mot3);
			if(errorStepperM3>0){
				Motor3_Step_Corrector[0] = (Mot3/errorStepperM3)+1;				// Round up to omit overshoot
				for(int i =0;i<(NUM_CORRECTOR_LOOPS-1);i++){
					if((errorStepperM3>0)&&(Motor3_Step_Corrector[i]>0)){
						errorStepperM3= errorStepperM3 -(1.0/(double)Motor3_Step_Corrector[i])*Mot3;	// second correction
					Motor3_Step_Corrector[i+1]=Mot3/errorStepperM3;
					}
				}
			}
		}
		if(Mot3==0){tmp_M3_Pause =0;}

#if RAMP_MODE_PREMIUM
		calcPremiumRamp(Mot1,Mot2,Mot3,tmp_M1_Pause,tmp_M2_Pause,tmp_M3_Pause,mostMotor);
#endif

#if OVERFLOW_HANDLING
		//////////////////////////////////////////////////////////////////
		///  OVERFLOW HANDLING
		//////////////////////////////////////////////////////////////////

		do{		// If Modulo Overflow: Count how many INT16_MAX Overflows occur
			if (tmp_M1_Pause>=UINT16_MAX){
				tmp_M1_Pause=(tmp_M1_Pause-UINT16_MAX);
				tmp_M1_OF_CNT +=1;
			}
			if (tmp_M2_Pause>=UINT16_MAX){
				tmp_M2_Pause=(tmp_M2_Pause-UINT16_MAX);
				tmp_M2_OF_CNT +=1;
			}
			if (tmp_M3_Pause>=UINT16_MAX){
				tmp_M3_Pause=(tmp_M3_Pause-UINT16_MAX);
				tmp_M3_OF_CNT +=1;
			}
		}while((tmp_M1_Pause>=UINT16_MAX || tmp_M2_Pause>=UINT16_MAX||tmp_M3_Pause>=UINT16_MAX));

#else
		if((tmp_M1_Pause>=INT16_MAX || tmp_M2_Pause>=INT16_MAX||tmp_M3_Pause>=INT16_MAX)){
			printf("Error\n");
		};
#endif

#if RAMP_MODE_TWOSTEP
		tmp_M1_Ramp_Pause= 3*tmp_M1_Pause;
		tmp_M2_Ramp_Pause= 3*tmp_M2_Pause;
		tmp_M3_Ramp_Pause= 3*tmp_M3_Pause;

		do{		// If Modulo Overflow: Count how many INT16_MAX Overflows occur
			if (tmp_M1_Ramp_Pause>=UINT16_MAX){
				tmp_M1_Ramp_Pause=(tmp_M1_Ramp_Pause-UINT16_MAX);
				tmp_M1_Ramp_OF_CNT +=1;
			}
			if (tmp_M2_Ramp_Pause>=UINT16_MAX){
				tmp_M2_Ramp_Pause=(tmp_M2_Ramp_Pause-UINT16_MAX);
				tmp_M2_Ramp_OF_CNT +=1;
			}
			if (tmp_M3_Ramp_Pause>=UINT16_MAX){
				tmp_M3_Ramp_Pause=(tmp_M3_Ramp_Pause-UINT16_MAX);
				tmp_M3_Ramp_OF_CNT +=1;
			}
		}while((tmp_M1_Ramp_Pause>=UINT16_MAX || tmp_M2_Ramp_Pause>=UINT16_MAX||tmp_M3_Ramp_Pause>=UINT16_MAX));
#endif

		//////////////////////////////////////////////////////////////////
		///  CALCULTATE INITIAL PAUSE TICKS
		//////////////////////////////////////////////////////////////////
//#if (!RAMP_ACTIVE || RAMP_MODE_PS)
#if RAMP_MODE_PS || !RAMP_ACTIVE
		if((tmp_M1_OF_CNT%2 == 0)||(tmp_M1_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M1_Pause_init = tmp_M1_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M1_Pause_init = (UINT16_MAX/2)+(tmp_M1_Pause/2);
		}

		if((tmp_M2_OF_CNT%2 == 0)||(tmp_M2_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M2_Pause_init = tmp_M2_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M2_Pause_init = (UINT16_MAX/2)+(tmp_M2_Pause/2);
		}

		if((tmp_M3_OF_CNT%2 == 0)||(tmp_M3_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M3_Pause_init = tmp_M3_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M3_Pause_init = (UINT16_MAX/2)+(tmp_M3_Pause/2);
		}
#endif

#if 1
		// Calculate init Values

		if((tmp_M1_OF_CNT%2 == 0)||(tmp_M1_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M1_Pause_init = tmp_M1_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M1_Pause_init = (UINT16_MAX/2)+(tmp_M1_Pause/2);
		}

		if((tmp_M2_OF_CNT%2 == 0)||(tmp_M2_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M2_Pause_init = tmp_M2_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M2_Pause_init = (UINT16_MAX/2)+(tmp_M2_Pause/2);
		}

		if((tmp_M3_OF_CNT%2 == 0)||(tmp_M3_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M3_Pause_init = tmp_M3_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M3_Pause_init = (UINT16_MAX/2)+(tmp_M3_Pause/2);
		}

#endif


#if RAMP_MODE_NSTEP

		int64_t ramp_tot_ticks = 0;					// Total Tick Numb uf Ramp
		int64_t tmp_ticks = 0;
		int64_t tmp_m1_pause_ticks[RAMP_NSTEPS];
		int64_t tmp_m1_pause_ticks_init[RAMP_NSTEPS];
		int64_t tmp_m1_pulse_ticks[RAMP_NSTEPS];
		int64_t tmp_m1_pulse_ticks_init[RAMP_NSTEPS];
		int64_t tmp_m2_pause_ticks[RAMP_NSTEPS];
		int64_t tmp_m2_pause_ticks_init[RAMP_NSTEPS];
		int64_t tmp_m2_pulse_ticks[RAMP_NSTEPS];
		int64_t tmp_m2_pulse_ticks_init[RAMP_NSTEPS];
		int64_t tmp_m3_pause_ticks[RAMP_NSTEPS];
		int64_t tmp_m3_pause_ticks_init[RAMP_NSTEPS];
		int64_t tmp_m3_pulse_ticks[RAMP_NSTEPS];
		int64_t tmp_m3_pulse_ticks_init[RAMP_NSTEPS];


		// Analyze Sequence
		// STEP 1
		if (mostMotor == 1){
			Ramp_M1_Start_State[0]=true;
			Ramp_M2_Start_State[0]=false;
			Ramp_M3_Start_State[0]=false;
		}else if(mostMotor == 2){
			Ramp_M1_Start_State[0]=false;
			Ramp_M2_Start_State[0]=true;
			Ramp_M3_Start_State[0]=false;
		}else if(mostMotor == 3){
			Ramp_M1_Start_State[0]=false;
			Ramp_M2_Start_State[0]=false;
			Ramp_M3_Start_State[0]=true;
		}


		int i = 0;

		if(tmp_M1_Pause_init>=RAMP_NSTEPS_STEPS){
			Ramp_M1_End_State[0]=Ramp_M1_Start_State[0+1] = false;
		}else{
			tmp_ticks = RAMP_NSTEPS_STEPS-tmp_M1_Pause_init;
			calcEndPoint(tmp_ticks,Ramp_M1_Start_State[i],&Ramp_M1_End_State[i],&Ramp_M2_End_Rem_Ticks[i]); // inclusive End_State and Rem Ticks
		}


		//////////////////////////////////////////////////////////////////
		///  MOTOR 1
		//////////////////////////////////////////////////////////////////

		// Set Globals and add Percentage

		Ramp_Ticks[RAMP_NSTEPS-1]=RAMP_NSTEPS_STEPS+scalePercent(RAMP_NSTEPS_STEPS,RAMP_NSTEPS_STEPS);
		tmp_m1_pause_ticks[RAMP_NSTEPS-1]=MOTOR_MINPAUSE_MOD_TICK+scalePercent(MOTOR_MINPAUSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
		tmp_m1_pulse_ticks[RAMP_NSTEPS-1]=MOTOR_PULSE_MOD_TICK+scalePercent(MOTOR_PULSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
		if (Ramp_M1_Start_State[RAMP_NSTEPS-1]== true){
			tmp_m1_pause_ticks_init[RAMP_NSTEPS-1] = 0;
			tmp_m1_pulse_ticks_init[RAMP_NSTEPS-1] = Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS-1]+scalePercent( Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS-1],RAMP_NSTEPS_STEP_TICKS);
		}else{
			tmp_m1_pause_ticks_init[RAMP_NSTEPS-1] = Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS-1]+scalePercent( Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS-1],RAMP_NSTEPS_STEP_TICKS);
			tmp_m1_pulse_ticks_init[RAMP_NSTEPS-1] = 0;
		}

		ramp_tot_ticks = Ramp_Ticks[RAMP_NSTEPS-1];

		for(int i=(RAMP_NSTEPS-2);i>=0;i--){

			Ramp_Ticks[i]=Ramp_Ticks[i+1]+scalePercent(RAMP_NSTEPS_STEPS,RAMP_NSTEPS_STEPS);
			tmp_m1_pause_ticks[i]=Ramp_M1_Pause_Ticks[i+1]+scalePercent(MOTOR_MINPAUSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
			tmp_m1_pulse_ticks[i]=Ramp_M1_Pulse_Ticks[i+1]+scalePercent(MOTOR_PULSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);

			if (Ramp_M1_Start_State[i]== true){
				tmp_m1_pause_ticks_init[i] = 0;
				tmp_m1_pulse_ticks_init[i] = Ramp_M1_End_Rem_Ticks[i]+scalePercent(Ramp_M1_End_Rem_Ticks[i],(RAMP_NSTEPS-i)*RAMP_NSTEPS_STEP_TICKS);
			}else{
				tmp_m1_pause_ticks_init[i] = Ramp_M1_End_Rem_Ticks[i]+scalePercent(Ramp_M1_End_Rem_Ticks[i],(RAMP_NSTEPS-i)*RAMP_NSTEPS_STEP_TICKS);
				tmp_m1_pulse_ticks_init[i] = 0;
			}

			ramp_tot_ticks +=Ramp_Ticks[i];

			overflowCounter_16bit(tmp_m1_pause_ticks_init[i],&Ramp_M1_Pause_Ticks_Init_OF[i],&Ramp_M1_Pause_Ticks_Init[i]);
			overflowCounter_16bit(tmp_m1_pause_ticks[i],&Ramp_M1_Pause_Ticks_OF[i],&Ramp_M1_Pause_Ticks[i]);
			overflowCounter_16bit(tmp_m1_pulse_ticks_init[i],&Ramp_M1_Pulse_Ticks_Init_OF[i],&Ramp_M1_Pulse_Ticks_Init[i]);
			overflowCounter_16bit(tmp_m1_pulse_ticks[i],&Ramp_M1_Pulse_Ticks_OF[i],&Ramp_M1_Pulse_Ticks[i]);
		}

		//////////////////////////////////////////////////////////////////
		///  MOTOR 2
		//////////////////////////////////////////////////////////////////

		// Set Globals and add Percentage


		tmp_m2_pause_ticks[RAMP_NSTEPS-1]=MOTOR_MINPAUSE_MOD_TICK+scalePercent(MOTOR_MINPAUSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
		tmp_m2_pulse_ticks[RAMP_NSTEPS-1]=MOTOR_PULSE_MOD_TICK+scalePercent(MOTOR_PULSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
		if (Ramp_M2_Start_State[RAMP_NSTEPS-1]== true){
			tmp_m2_pause_ticks_init[RAMP_NSTEPS-1] = 0;
			tmp_m2_pulse_ticks_init[RAMP_NSTEPS-1] = Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS-1]+scalePercent( Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS-1],RAMP_NSTEPS_STEP_TICKS);
		}else{
			tmp_m2_pause_ticks_init[RAMP_NSTEPS-1] = Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS-1]+scalePercent( Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS-1],RAMP_NSTEPS_STEP_TICKS);
			tmp_m2_pulse_ticks_init[RAMP_NSTEPS-1] = 0;
		}

		ramp_tot_ticks = Ramp_Ticks[RAMP_NSTEPS-1];

		for(int i=(RAMP_NSTEPS-2);i>=0;i--){

			Ramp_Ticks[i]=Ramp_Ticks[i+1]+scalePercent(RAMP_NSTEPS_STEPS,RAMP_NSTEPS_STEPS);
			tmp_m2_pause_ticks[i]=Ramp_M2_Pause_Ticks[i+1]+scalePercent(MOTOR_MINPAUSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);
			tmp_m2_pulse_ticks[i]=Ramp_M2_Pulse_Ticks[i+1]+scalePercent(MOTOR_PULSE_MOD_TICK,RAMP_NSTEPS_STEP_TICKS);

			if (Ramp_M2_Start_State[i]== true){
				tmp_m2_pause_ticks_init[i] = 0;
				tmp_m2_pulse_ticks_init[i] = Ramp_M2_End_Rem_Ticks[i]+scalePercent(Ramp_M2_End_Rem_Ticks[i],(RAMP_NSTEPS-i)*RAMP_NSTEPS_STEP_TICKS);
			}else{
				tmp_m2_pause_ticks_init[i] = Ramp_M2_End_Rem_Ticks[i]+scalePercent(Ramp_M2_End_Rem_Ticks[i],(RAMP_NSTEPS-i)*RAMP_NSTEPS_STEP_TICKS);
				tmp_m2_pulse_ticks_init[i] = 0;
			}

			ramp_tot_ticks +=Ramp_Ticks[i];

			overflowCounter_16bit(tmp_m2_pause_ticks_init[i],&Ramp_M2_Pause_Ticks_Init_OF[i],&Ramp_M2_Pause_Ticks_Init[i]);
			overflowCounter_16bit(tmp_m2_pause_ticks[i],&Ramp_M2_Pause_Ticks_OF[i],&Ramp_M2_Pause_Ticks[i]);
			overflowCounter_16bit(tmp_m2_pulse_ticks_init[i],&Ramp_M2_Pulse_Ticks_Init_OF[i],&Ramp_M2_Pulse_Ticks_Init[i]);
			overflowCounter_16bit(tmp_m2_pulse_ticks[i],&Ramp_M2_Pulse_Ticks_OF[i],&Ramp_M2_Pulse_Ticks[i]);
		}
#endif

#if !RAMP_MODE_NSTEP
		// Calculate init Values

		if((tmp_M1_Ramp_OF_CNT%2 == 0)||(tmp_M1_Ramp_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M1_Pause_init = tmp_M1_Ramp_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M1_Pause_init = (UINT16_MAX/2)+(tmp_M1_Ramp_Pause/2);
		}

		if((tmp_M2_Ramp_OF_CNT%2 == 0)||(tmp_M2_Ramp_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M2_Pause_init = tmp_M2_Ramp_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M2_Pause_init = (UINT16_MAX/2)+(tmp_M2_Ramp_Pause/2);
		}

		if((tmp_M3_Ramp_OF_CNT%2 == 0)||(tmp_M3_Ramp_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M3_Pause_init = tmp_M3_Ramp_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M3_Pause_init = (UINT16_MAX/2)+(tmp_M3_Ramp_Pause/2);
		}

#endif


		//////////////////////////////////////////////////////////////////
		///  SET INITIAL CHANNEL VALUES
		//////////////////////////////////////////////////////////////////
#if !RAMP_MODE_NSTEP
		// Set Initial Pause Value
		if(Mot1 !=0){
			if(mostMotor==1){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M1_Pause_init);}
		}
		if(Mot2 !=0){
			if(mostMotor==2){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M2_Pause_init);}
		}
		if(Mot3!=0){
			if(mostMotor==3){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M3_Pause_init);}
		}
#endif

#if RAMP_MODE_NSTEP
		// Set Initial Pause Value
		if(Mot1 !=0){
			if(mostMotor==1){FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Ramp_M1_Pause_Ticks_Init[0]);}
		}
		if(Mot2 !=0){
			if(mostMotor==2){FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Ramp_M2_Pause_Ticks_Init[0]);}
		}
		if(Mot3!=0){
			if(mostMotor==3){FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(Ramp_M3_Pause_Ticks_Init[0]);}
		}
#endif
		//////////////////////////////////////////////////////////////////
		///  SET OUTPUTS TO GLOBALS
		//////////////////////////////////////////////////////////////////

		Motor1_Step_OF = tmp_M1_OF_CNT;
		Motor2_Step_OF = tmp_M2_OF_CNT;
		Motor3_Step_OF = tmp_M3_OF_CNT;

		Motor1_Step_OF_Curr = tmp_M1_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)
		Motor2_Step_OF_Curr = tmp_M2_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)
		Motor3_Step_OF_Curr = tmp_M3_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)

#if RAMP_MODE_TWOSTEP
		M1_Step_Ramp_OF = tmp_M1_Ramp_OF_CNT;
		M2_Step_Ramp_OF = tmp_M2_Ramp_OF_CNT;
		M3_Step_Ramp_OF = tmp_M3_Ramp_OF_CNT;


		M1_Step_Ramp_OF_Curr = tmp_M1_Ramp_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)
		M2_Step_Ramp_OF_Curr = tmp_M2_Ramp_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)
		M3_Step_Ramp_OF_Curr = tmp_M3_Ramp_OF_CNT/2;	// set Initial Pause to half (in case uneven -> see tmp_Mx_Pause_init)
#endif


		Motor1_Pause = (uint16_t)tmp_M1_Pause;
		Motor2_Pause = (uint16_t)tmp_M2_Pause;
		Motor3_Pause = (uint16_t)tmp_M3_Pause;

		M1_Pause_min = (uint16_t)tmp_M1_Pause;
		M2_Pause_min = (uint16_t)tmp_M2_Pause;
		M3_Pause_min = (uint16_t)tmp_M3_Pause;

		M1_Pause_Ramp = (uint16_t)tmp_M1_Pause;
		M2_Pause_Ramp = (uint16_t)tmp_M2_Pause;
		M3_Pause_Ramp = (uint16_t)tmp_M3_Pause;

		return mostMotor;	// no Error
}

#if RAMP_MODE_PREMIUM
/*
 * Function: Calc Premium Ramp
 * ----------------------------
 *   Calculates a linear Ramp for three Motors with different step Numbers
 *
 *   Motx: Positive Number of steps in Total
 *   pausex: Ticknumber of Pause in Full speed
 *   mostMotor: index for Motor with most steps
 */
static void calcLinearRamp(int32_t Mot1,int32_t Mot2, int32_t Mot3,int64_t pause1,int64_t pause2,int64_t pause3, uint32_t mostMotor){

	// The Time for the starting Ramp is the product of the Ticknumber without ramp multiplied by the Factor RAMP_DISTANCE_FACTOR
	// Version 2: Pulse times stay the same. the Tick increase during a pulse is added to the following pause
#define Version 2
	uint64_t totalTicksRamp = 0;
	uint64_t totalTicksNORamp = 0;
	uint64_t tmpTotalPauseTicks = 0;
	uint64_t tmpSteps = 0;
	uint64_t tmpTicks = 0;
	uint64_t tmpRestPause = 0;
	double incRate = ADD_TICK_PER_STEP/(MOTOR_PULSE_MOD_TICK+MOTOR_MINPAUSE_MOD_TICK);	// increasingRate
	double incTickPulse = incRate*MOTOR_PULSE_MOD_TICK;	// Tickincrease per Step

	//////////////////////////////////////////////////////////////////
	///  Calc Lenth of Ramp in Ticks
	//////////////////////////////////////////////////////////////////
#if Version == 1
	totalTicksRamp = (int64_t)MOTOR_PULSE_MOD_TICK*RAMP_NUMB_STEPS; // Pulse Ticks with Ramp
	int tmp_pauseAdd = MOTOR_MINPAUSE_MOD_TICK;						// last

	for(int i=0;i<RAMP_NUMB_STEPS;i++){
		tmp_pauseAdd += ADD_TICK_PER_STEP;
		totalTicksRamp += tmp_pauseAdd
	}

	totalTicksNORamp = RAMP_NUMB_STEPS*(MOTOR_MINPAUSE_MOD_TICK+MOTOR_PULSE_MOD_TICK);
#endif
#if Version == 2
	totalTicksRamp = ((int64_t)MOTOR_PULSE_MOD_TICK+(int64_t)MOTOR_MINPAUSE_MOD_TICK)*RAMP_NUMB_STEPS; // Pulse Ticks with Ramp
	totalTicksNORamp = RAMP_NUMB_STEPS*(MOTOR_MINPAUSE_MOD_TICK+MOTOR_PULSE_MOD_TICK);
#endif

	//////////////////////////////////////////////////////////////////
	///  Calc Pause Times and Overflow Counts
	//////////////////////////////////////////////////////////////////
#if Version == 1
	if (mostMotor == 1){
		tmpTotalPauseTicks = pause1;
		overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[RAMP_NUMB_STEPS-1],&M1_PR_Ramp_Pause[RAMP_NUMB_STEPS-1]);
		for(int i=RAMP_NUMB_STEPS-2;i>=0;i--){
			tmpTotalPauseTicks+=ADD_TICK_PER_STEP;
			overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[i],&M1_PR_Ramp_Pause[i]);
			}
	}else{
		// Calculate Steps in Ramp and last Pause
		tmpTicks = totalTicksRamp - pause1/2; //Subtract initial Pause
		while(tmpTicks>0){
			tmpRestPause=tmpTicks;
			tmpTicks-=(MOTOR_PULSE_MOD_TICK+pause1);	// subtract next pause
			tmpSteps++; 								// TODO: What if Pause is in Pulse???
		}
		tmpRestPause*=(1.0f+incRatePause);
		overflowCounter_16bit(tmpRestPause,&M1_PR_Ramp_OF[tmpSteps-1],&M1_PR_Ramp_Pause[tmpSteps-1]);
														// TODO: Rest Ticks must be longer
														// TODO: Increase in Pulse has to be calculated too

		for(int i=tmpSteps-1;i>=0;i--){
			tmpTotalPauseTicks+=ADD_TICK_PER_STEP;
			overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[i],&M1_PR_Ramp_Pause[i]);
					}

	}
#endif
#if Version == 2
	if (mostMotor == 1){
			tmpTotalPauseTicks = pause1;
			tmpTotalPauseTicks += tmpTotalPauseTicks(1+incRate)+incTickPulse; // Todo: Add Pulse here? Right for the next step??
			overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[RAMP_NUMB_STEPS-1],&M1_PR_Ramp_Pause[RAMP_NUMB_STEPS-1]);
			for(int i=RAMP_NUMB_STEPS-2;i>=0;i--){
				tmpTotalPauseTicks+=ADD_TICK_PER_STEP;
				overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[i],&M1_PR_Ramp_Pause[i]);
				}
		}else{
#endif


		tmpSteps =


		for(int i=RAMP_NUMB_STEPS-1;i>=0;i--){

			tmpTotalPauseTicks+=ADD_TICK_PER_STEP;
			overflowCounter_16bit(tmpTotalPauseTicks,&M1_PR_Ramp_OF[i],&M1_PR_Ramp_Pause[i]);
			}

	}



	M1_PR_Ramp_Pause[1];
	M1_PR_Ramp_Pause[1];

}
#endif

/*
 * Function: 16 bit overflow counter
 * ----------------------------
 *   Counts number of 16-bit unsigned Overflows occur
 *
 *   number: Number under test
 *   OF_counter: how many Overflows occur until number is reached
 *   modulo: rest number after last overflow occured
 */
static void overflowCounter_16bit(uint64_t number, uint16_t* OF_counter, uint16_t* modulo){
	do{		// If Modulo Overflow: Count how many INT16_MAX Overflows occur
		if (number>=UINT16_MAX){
			number=(number-UINT16_MAX);
			(*OF_counter)+=1;
		}
	}while(number>=UINT16_MAX);
	(*modulo) = (uint16_t)number;
}

/*
 * Function: Gives Back Percentage of input
 * ----------------------------
 *   calculates a Part of an Int64 by Percentage
 *
 *   Value: Number Reprecenting 100%
 *   Percentage: Percennt[%]
 *   return: Percentage of value
 */
uint64_t scalePercent(uint64_t value, int32_t percentage){
	uint64_t result;
	result=value*percentage/100;
	return result;
}

/*
 * Function: Calc last remaining Pulse/Pause
 * ----------------------------
 *   calculates a Part of an Int64 by Percentage
 *
 *   ticks: Total Amount of Ticks
 *   start State: true = high = Pulse
 *   endState: (Reference) Return end State true = high = Pulse
 *   end Ticks: Remaining Ticks in end State
 */
void calcEndPoint(uint64_t ticks, bool startState, bool* endState, int64_t* endTicks){

	if (startState == true){
		do{
			if(ticks<=MOTOR_PULSE_MOD_TICK){
				*endState = true;
				*endTicks= MOTOR_PULSE_MOD_TICK- ticks;
				break;
			}else{ticks -= MOTOR_PULSE_MOD_TICK;}

			if(ticks<=MOTOR_MINPAUSE_MOD_TICK){
				*endState = false;
				*endTicks=MOTOR_MINPAUSE_MOD_TICK- ticks;
				break;
			}else{ticks -= MOTOR_MINPAUSE_MOD_TICK;}
		}while(ticks>0);
	}else{
		do{
			if(ticks<=MOTOR_MINPAUSE_MOD_TICK){
				*endState = false;
				*endTicks=MOTOR_MINPAUSE_MOD_TICK- ticks;
				break;
			}else{ticks -= MOTOR_MINPAUSE_MOD_TICK;}

			if(ticks<=MOTOR_PULSE_MOD_TICK){
				*endState = true;
				*endTicks=MOTOR_PULSE_MOD_TICK- ticks;
				break;
			}else{ticks -= MOTOR_PULSE_MOD_TICK;}

		}while(ticks>0);
	}
}
