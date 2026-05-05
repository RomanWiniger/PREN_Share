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
#include "control.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "globals.h"

#if RAMP_MODE_NSTEP
static void overflowCounter_16bit(uint64_t number, uint16_t* OF_counter, uint16_t* modulo);
void calcEndPoint(int64_t total_ticks,bool start_state,uint64_t rem_start_ticks,uint64_t highTicks,uint64_t lowTicks,bool* endState,uint64_t* endTicks);
double scalePercent(uint64_t value, uint64_t percentage);
void segmentEndState(int64_t total_ticks,bool start_state,uint64_t start_state_ticks,uint64_t other_state_ticks,bool* endState,uint64_t* endTicks);
void analyzeSegment(bool startState[RAMP_NSTEPS+1],uint64_t initTicks,uint64_t endTicks[RAMP_NSTEPS+1],
					uint64_t pulseTicks[RAMP_NSTEPS+1], uint64_t pauseTicks[RAMP_NSTEPS+1],
					double factors[RAMP_NSTEPS+1],uint64_t pulseOrigin,uint64_t pauseOrigin);
void resolveOverflows(	uint64_t rem64[RAMP_NSTEPS+1],uint64_t pulse64[RAMP_NSTEPS+1], uint64_t pause64[RAMP_NSTEPS+1],
					uint16_t rem16[RAMP_NSTEPS+1],uint16_t pulse16[RAMP_NSTEPS+1], uint16_t pause16[RAMP_NSTEPS+1],
					uint16_t remOF[RAMP_NSTEPS+1],uint16_t pulseOF[RAMP_NSTEPS+1], uint16_t pauseOF[RAMP_NSTEPS+1]);
void calcCheckPending(bool pend[RAMP_NSTEPS+1],uint16_t tickEnd[RAMP_NSTEPS]);
#endif



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
			#if RAMP_MODE_NSTEP
	    		Motor1_Pause_Full = MOTOR_MINPAUSE_MOD_TICK;
			#endif
			tmp_M1_Pause = MOTOR_MINPAUSE_MOD_TICK;
			totalTicks=((int64_t)tmp_M1_Pause +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot1;

		}else{
			if (Mot2>=Mot1 && Mot2>=Mot3){
				mostMotor=2;
				#if RAMP_MODE_NSTEP
					Motor2_Pause_Full = MOTOR_MINPAUSE_MOD_TICK;
				#endif

				tmp_M2_Pause = MOTOR_MINPAUSE_MOD_TICK;
				totalTicks=((int64_t)MOTOR_MINPAUSE_MOD_TICK +(int64_t)MOTOR_PULSE_MOD_TICK)* (int64_t)Mot2;
			}else{
				if (Mot3>=Mot2 && Mot3>=Mot1){
						mostMotor=3;
						#if RAMP_MODE_NSTEP
							Motor3_Pause_Full = MOTOR_MINPAUSE_MOD_TICK;
						#endif
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
			#if RAMP_MODE_NSTEP
				Motor1_Pause_Full=tmp_M1_Pause;
			#endif
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
			#if RAMP_MODE_NSTEP
				Motor2_Pause_Full=tmp_M2_Pause;
			#endif
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
			#if RAMP_MODE_NSTEP
				Motor3_Pause_Full=tmp_M3_Pause;
			#endif
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
		//////////////////////////////////////////////////////////////////
		///  SET GLOBALS FOR RAMP (NSTEP)
		//////////////////////////////////////////////////////////////////
		//
		// 	Example:
		//		                ____               |                ____                                ____
		//	M1	_______________|    |______________|_______________|    |______________________________|    |____________
		//		       ____                  ____  |               ____                  ____                  ____
		//	M2	______|    |________________|    |_|______________|    |________________|    |________________|    |_____
		//		 ____       ____       ____       _|__       ____       ____       ____       ____       ____       ____
		// 	M3	|    |_____|    |_____|    |_____| |  |_____|    |_____|    |_____|    |_____|    |_____|    |_____|    |
		//                                         |
		// 		|----------------------------------|----------------------------------|----------------------------------|
		//					Segment 1							Segment 2						Segment n
		//
		//  Start Sequence is devided in Segments of [RAMP_NSTEPS_STEPS} Ticks
		//  At the End of each Segment there is a End_State to indicate if the pin is high or low
		//	the Parameter Ramp_Mx_End_Rem_Ticks indicates how many ticks are remainig in the following state
		//
		//	Example of an expanded Segment
		//		                       ____              |
		//	M1	______________________|    |_____________|_________
		//		        _____                     ____   |
		//	M2	_______|     |___________________|    |__|__________
		//		 _____        _____        _____        _|__       _
		// 	M3	|     |______|     |______|     |______| |  |_____|
		//                                               |
		// 		|----------------------------------------|----------
		// 					Segment 1 = Segment 2 + x%
		//					x = RAMP_NSTEPS_STEP_TICKS
		//					The increase is refered to the following Segment

		double Ramp_Segment_factor[RAMP_NSTEPS+1];
		uint64_t tmp_Ramp_Segment_ticks;

		uint64_t tmp_m1_ticks_init = 0;
		uint64_t tmp_m1_pause_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m1_pulse_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m1_end_rem_ticks[RAMP_NSTEPS+1] = {0};

		uint64_t tmp_m2_ticks_init = 0;
		uint64_t tmp_m2_pulse_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m2_pause_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m2_end_rem_ticks[RAMP_NSTEPS+1] = {0};

		uint64_t tmp_m3_ticks_init = 0;
		uint64_t tmp_m3_pause_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m3_pulse_ticks[RAMP_NSTEPS+1] = {0};
		uint64_t tmp_m3_end_rem_ticks[RAMP_NSTEPS+1] = {0};

		// Calc Ramp Factors
		Ramp_Segment_factor[RAMP_NSTEPS]=1+scalePercent(1,RAMP_NSTEPS_STEP_PERC); // TODO:Immer gleiches Resultat: Temp Berechnen
		for(int i = (RAMP_NSTEPS-1);i>=0;i--){
			Ramp_Segment_factor[i]=Ramp_Segment_factor[i+1]+scalePercent(1,RAMP_NSTEPS_STEP_PERC);
		}

		for(int i = 0;i<=RAMP_NSTEPS;i++){
			tmp_Ramp_Segment_ticks=Ramp_Segment_factor[i]*RAMP_NSTEPS_STEPS;
			overflowCounter_16bit(tmp_Ramp_Segment_ticks,&Ramp_Step_Ticks_OF[i],&Ramp_Step_Ticks[i]);
			Ramp_Step_Ticks_OF_Curr[i]=Ramp_Step_Ticks_OF[i]; // initialize Overflow Current of Ramp
		}
		// Step 0 is skipped: Ramp_Step_Curr is initialised to 1 below.
		// Do NOT set Ramp_Step_Ticks[0] to a tiny value – the FTM compare
		// fires on CNT==CnV (exact equality only).  A value smaller than the
		// ISR execution time causes CNT to overshoot the new CnV, so the match
		// is missed and the interrupt is delayed by a full 65535-tick wrap.
		Ramp_Step_Ticks_OF[0]=0;
		Ramp_Step_Ticks[0]=0;


		// Set first Step State of all Motors
		if (mostMotor == 1){
			Ramp_M1_Start_State[1]=false;
			tmp_m1_ticks_init=0;
			Ramp_M2_Start_State[1]=false;
			tmp_m2_ticks_init=tmp_M2_Pause_init;
			Ramp_M3_Start_State[1]=false;
			tmp_m3_ticks_init=tmp_M3_Pause_init;
		}else if(mostMotor == 2){
			Ramp_M1_Start_State[1]=false;
			tmp_m1_ticks_init=tmp_M1_Pause_init;
			Ramp_M2_Start_State[1]=false;
			tmp_m2_ticks_init=0;
			Ramp_M3_Start_State[1]=false;
			tmp_m3_ticks_init=tmp_M3_Pause_init;
		}else if(mostMotor == 3){
			Ramp_M1_Start_State[1]=false;
			tmp_m1_ticks_init=tmp_M1_Pause_init;
			Ramp_M2_Start_State[1]=false;
			tmp_m2_ticks_init=tmp_M2_Pause_init;
			Ramp_M3_Start_State[1]=false;
			tmp_m3_ticks_init=0;
		}

		//////////////////////////////////////////////////////////////////
		///  MOTOR 1
		//////////////////////////////////////////////////////////////////

		analyzeSegment(Ramp_M1_Start_State, tmp_m1_ticks_init,tmp_m1_end_rem_ticks, tmp_m1_pulse_ticks,tmp_m1_pause_ticks, Ramp_Segment_factor,MOTOR_PULSE_MOD_TICK,Motor1_Pause_Full);
		resolveOverflows(tmp_m1_end_rem_ticks, tmp_m1_pulse_ticks,tmp_m1_pause_ticks,Ramp_M1_End_Rem_Ticks ,Ramp_M1_Pulse_Ticks,Ramp_M1_Pause_Ticks,Ramp_M1_End_Rem_Ticks_OF,Ramp_M1_Pulse_Ticks_OF,Ramp_M1_Pause_Ticks_OF);
		//calcCheckPending(Ramp_M1_Rem_Pending,Ramp_M1_End_Rem_Ticks);

		//////////////////////////////////////////////////////////////////
		///  MOTOR 2
		//////////////////////////////////////////////////////////////////

		analyzeSegment(Ramp_M2_Start_State,tmp_m2_ticks_init,tmp_m2_end_rem_ticks, tmp_m2_pulse_ticks,tmp_m2_pause_ticks, Ramp_Segment_factor,MOTOR_PULSE_MOD_TICK,Motor2_Pause_Full);
		resolveOverflows(tmp_m2_end_rem_ticks, tmp_m2_pulse_ticks,tmp_m2_pause_ticks,Ramp_M2_End_Rem_Ticks ,Ramp_M2_Pulse_Ticks,Ramp_M2_Pause_Ticks,Ramp_M2_End_Rem_Ticks_OF,Ramp_M2_Pulse_Ticks_OF,Ramp_M2_Pause_Ticks_OF);
		//calcCheckPending(Ramp_M2_Rem_Pending,Ramp_M2_End_Rem_Ticks);

		//////////////////////////////////////////////////////////////////
		///  MOTOR 3
		//////////////////////////////////////////////////////////////////

		analyzeSegment(Ramp_M3_Start_State,tmp_m3_ticks_init,tmp_m3_end_rem_ticks, tmp_m3_pulse_ticks,tmp_m3_pause_ticks, Ramp_Segment_factor,MOTOR_PULSE_MOD_TICK,Motor3_Pause_Full);
		resolveOverflows(tmp_m3_end_rem_ticks, tmp_m3_pulse_ticks,tmp_m3_pause_ticks,Ramp_M3_End_Rem_Ticks ,Ramp_M3_Pulse_Ticks,Ramp_M3_Pause_Ticks,Ramp_M3_End_Rem_Ticks_OF,Ramp_M3_Pulse_Ticks_OF,Ramp_M3_Pause_Ticks_OF);
		//calcCheckPending(Ramp_M3_Rem_Pending,Ramp_M3_End_Rem_Ticks);

		// Pre-load all _OF_Curr arrays from _OF so ISR decrements never corrupt the reset values.
		for(int i = 0; i <= RAMP_NSTEPS; i++){
			Ramp_M1_Pulse_Ticks_OF_Curr[i]    = Ramp_M1_Pulse_Ticks_OF[i];
			Ramp_M1_Pause_Ticks_OF_Curr[i]    = Ramp_M1_Pause_Ticks_OF[i];
			Ramp_M1_End_Rem_Ticks_OF_Curr[i]  = Ramp_M1_End_Rem_Ticks_OF[i];
			Ramp_M2_Pulse_Ticks_OF_Curr[i]    = Ramp_M2_Pulse_Ticks_OF[i];
			Ramp_M2_Pause_Ticks_OF_Curr[i]    = Ramp_M2_Pause_Ticks_OF[i];
			Ramp_M2_End_Rem_Ticks_OF_Curr[i]  = Ramp_M2_End_Rem_Ticks_OF[i];
			Ramp_M3_Pulse_Ticks_OF_Curr[i]    = Ramp_M3_Pulse_Ticks_OF[i];
			Ramp_M3_Pause_Ticks_OF_Curr[i]    = Ramp_M3_Pause_Ticks_OF[i];
			Ramp_M3_End_Rem_Ticks_OF_Curr[i]  = Ramp_M3_End_Rem_Ticks_OF[i];
		}
#endif

#if !RAMP_MODE_NSTEP
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

		if((tmp_M3_OF_CNT%2 == 0)||(tmp_M2_OF_CNT==0)){ 	// Even Overflow Counter
			tmp_M3_Pause_init = tmp_M3_Pause/2;
		}else{						// Odd Overflow Counter
			tmp_M3_Pause_init = (UINT16_MAX/2)+(tmp_M3_Pause/2);
		}

#endif


		//////////////////////////////////////////////////////////////////
		///  SET INITIAL CHANNEL VALUES
		//////////////////////////////////////////////////////////////////
#if !RAMP_MODE_NSTEP
		// Set Initial Pause Value
		if(Mot1 !=0){
			if(mostMotor==1){FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M1_Pause_init);}
		}
		if(Mot2 !=0){
			if(mostMotor==2){FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M2_Pause_init);}
		}
		if(Mot3!=0){
			if(mostMotor==3){FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV = FIRST_PULSE_START_MOD;}else{FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV =FIRST_PULSE_START_MOD+(tmp_M3_Pause_init);}
		}
#endif

#if RAMP_MODE_NSTEP
		// Start at step 1 so the first CH6 firing uses Ramp_Step_Ticks[1] (large
		// enough that CnV += N never overshoots CNT inside the ISR).
		Ramp_Step_Curr = 1;

		// Set Initial Pause Value
		if(Mot1 !=0){
			FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV =RAMP_NSTEPS_FIRST_MOD+100;
		}
		if(Mot2 !=0){
			FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV =RAMP_NSTEPS_FIRST_MOD+100;
		}
		if(Mot3!=0){
			FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV =RAMP_NSTEPS_FIRST_MOD+100;
		}

		FTM0->CONTROLS[6].CnV=RAMP_NSTEPS_FIRST_MOD; // earliest ISR -> will set CHnV for Motors above
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
#endif

#if RAMP_MODE_NSTEP
void analyzeSegment(bool startState[RAMP_NSTEPS+1],uint64_t initTicks,uint64_t endTicks[RAMP_NSTEPS+1],uint64_t pulseTicks[RAMP_NSTEPS+1], uint64_t pauseTicks[RAMP_NSTEPS+1],double factors[RAMP_NSTEPS+1],uint64_t pulseOrigin,uint64_t pauseOrigin){
	//First Segment
	endTicks[0]=initTicks;
	calcEndPoint(RAMP_NSTEPS_STEPS,startState[1],endTicks[0],pulseOrigin,pauseOrigin,&startState[2],&endTicks[1]);

	//Any further segment
	for(int i=2;i<=RAMP_NSTEPS;i++){
		calcEndPoint(RAMP_NSTEPS_STEPS,startState[i],endTicks[i-1],pulseOrigin,pauseOrigin,&startState[i+1],&endTicks[i]);
	}

	// Scale-Up each Segment
	for(int i=0;i<=RAMP_NSTEPS;i++){
        endTicks[i]   *= factors[i];
        pulseTicks[i] = pulseOrigin * factors[i];
        pauseTicks[i] = pauseOrigin * factors[i];
	}
}
#endif

#if RAMP_MODE_NSTEP
void calcEndPoint(	int64_t total_ticks,
					bool start_state,
					uint64_t rem_start_ticks,
					uint64_t highTicks,
					uint64_t lowTicks,
					bool* endState, 		// Output
					uint64_t* endTicks 		// Output
					){

	// Case: Remaining Ticks are longer than the total_tick (Segment)
	if(rem_start_ticks>total_ticks){
		*endState = start_state;
		*endTicks = rem_start_ticks - total_ticks;
		return;
	}

	// if not, subtract the first remaining ticks
	total_ticks -= rem_start_ticks;


	// Calculate the End State of the Segment
	if(start_state){
		segmentEndState((uint64_t)total_ticks,start_state,highTicks,lowTicks,endState,endTicks);
	}else{
		segmentEndState((uint64_t)total_ticks,start_state,lowTicks,highTicks,endState,endTicks);
	}
}
#endif

#if RAMP_MODE_NSTEP
/*
 * Function: Calc last remaining Pulse/Pause
 * ----------------------------
 *   Same as "calcEndPoint" bit with no initial remaining steps
 *
 *   total_ticks: Total Amount of Ticks
 *   start State: high = true
 *   start_state_ticks: Pulsewith (hig/low) in ticks in start_state
 *   other_state_ticks: Pulsewith (hig/low) in ticks in inverse start_State
 *   endState: OUTPUT State at the End of Segment
 *   endTicks: OUTPUT Remaining Ticks in endState (End of Segment)
 */
void segmentEndState(	int64_t total_ticks,
						bool start_state,
						uint64_t start_state_ticks,
						uint64_t other_state_ticks,
						bool* endState,
						uint64_t* endTicks){
	bool tmp_state = start_state;

	while (true){
		// Start State
		if(total_ticks < start_state_ticks){
			*endState = tmp_state;
			*endTicks = start_state_ticks - total_ticks;
			return;
		}else{
			total_ticks -= start_state_ticks;
			tmp_state = !tmp_state;
		}

		// Other State
		if(total_ticks < other_state_ticks){
			*endState = tmp_state;
			*endTicks = other_state_ticks - total_ticks;
			return;
		}else{
			total_ticks -= other_state_ticks;
			tmp_state = !tmp_state;
		}
	}
}
#endif

#if RAMP_MODE_NSTEP
/*
 * Function: resolfe Overflows for one Motor-Ramp
 * ----------------------------
 *   calculates OF Counter to 16bit unsigned for Ramp-Values
 *
 *   number: Number under test
 *   OF_counter: how many Overflows occur until number is reached
 *   modulo: rest number after last overflow occured
 */
void resolveOverflows(	uint64_t rem64[RAMP_NSTEPS+1],uint64_t pulse64[RAMP_NSTEPS+1], uint64_t pause64[RAMP_NSTEPS+1],
						uint16_t rem16[RAMP_NSTEPS+1],uint16_t pulse16[RAMP_NSTEPS+1], uint16_t pause16[RAMP_NSTEPS+1],
						uint16_t remOF[RAMP_NSTEPS+1],uint16_t pulseOF[RAMP_NSTEPS+1], uint16_t pauseOF[RAMP_NSTEPS+1]){
	for(int i=0;i<=RAMP_NSTEPS;i++){
		overflowCounter_16bit(rem64[i],&remOF[i],&rem16[i]);
		overflowCounter_16bit(pulse64[i],&pulseOF[i],&pulse16[i]);
		overflowCounter_16bit(pause64[i],&pauseOF[i],&pause16[i]);
	};
}
#endif


#if RAMP_MODE_NSTEP
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
	(*OF_counter)=0; //init with zero
	do{		// If Modulo Overflow: Count how many INT16_MAX Overflows occur
		if (number>=UINT16_MAX){
			number-=UINT16_MAX;
			(*OF_counter)+=1;
		}
	}while(number>=UINT16_MAX);

	// Asign rest
	*modulo = (uint16_t)number;
}
#endif

#if RAMP_MODE_NSTEP
/*
 * Function: Gives Back Percentage of input
 * ----------------------------
 *   calculates a Part of an Int64 by Percentage
 *
 *   Value: Number Reprecenting 100%
 *   Percentage: Percennt[%]
 *   return: Percentage of value
 */
double scalePercent(uint64_t value, uint64_t percentage){
	double result;
	result=(double)value*(double)percentage/100.0;
	return result;
}
#endif

#if 0
/*
 * Function: set Pend if Step
 * ----------------------------
 *   calc the End state of a Pulsesegment with high and low pulses
 *   and an individual initial tick number
 *
 *   pend: OUTPUT Array of bool for pending flag
 *   stepEnd: Array of Remaining Ticks at End of Step
 */
void calcCheckPending(bool pend[RAMP_NSTEPS+1],uint16_t tickEnd[RAMP_NSTEPS+1]){

	for(int i = 1;i<=RAMP_NSTEPS;i++){
		pend[i]=(tickEnd[i-1]>0);
	}

}
#endif

