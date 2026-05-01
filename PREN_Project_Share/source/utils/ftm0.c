/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Common settings of the FTM0
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          04.04.2020
 *
 *--------------------------------------------------------------------
 */

#include "platform.h"
#include "ftm0.h"
#include "globals.h"
#include "motor_config.h"
#if DEBUG_MODE
	#include "reserve_pin_config.h"
#endif

/**
 * Default handler is called if there is no handler for the FTM0 channel or tof interrupt
 */
void Default_Handler_FTM0()
{
  __asm("bkpt"); // Still a hacker? ;-)
}

//Used Channels:
void FTM0CH0_IRQHandler(void);
void FTM0CH1_IRQHandler(void); // Motor 1
void FTM0CH2_IRQHandler(void); // Motor 2
void FTM0CH4_IRQHandler(void); // Motor 3
void FTM0CH5_IRQHandler(void); // Rotation Motor
#if RAMP_MODE_NSTEP
void FTM0CH6_IRQHandler(void); // Ramp Step incrementer
#endif
void FTM0TOF_IRQHandler(void);

// not used Channels
//void FTM0CH0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
//void FTM0CH1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
//void FTM0CH2_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
void FTM0CH3_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
//void FTM0CH4_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
//void FTM0CH6_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));void FTM0CH5_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
#if !RAMP_MODE_NSTEP
void FTM0CH6_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
#endif
void FTM0CH7_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));
//void FTM0TOF_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler_FTM0")));


#define CHF_CHIE_MASK       (FTM_CnSC_CHF_MASK | FTM_CnSC_CHIE_MASK)
#define TOF_TOIE_MASK       (FTM_SC_TOF_MASK | FTM_SC_TOIE_MASK)

/**
 * Interrupt handler to distribute the different interrupt sources of the FTM0:
 * - channel 0..7
 * - timer overflow
 */
void FTM0_IRQHandler(void)
{
  if ((FTM0->CONTROLS[0].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH0_IRQHandler();
  if ((FTM0->CONTROLS[1].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH1_IRQHandler();
  if ((FTM0->CONTROLS[2].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH2_IRQHandler();
  if ((FTM0->CONTROLS[3].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH3_IRQHandler();
  if ((FTM0->CONTROLS[4].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH4_IRQHandler();
  if ((FTM0->CONTROLS[5].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH5_IRQHandler();
  if ((FTM0->CONTROLS[6].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH6_IRQHandler();
  if ((FTM0->CONTROLS[7].CnSC & CHF_CHIE_MASK) == CHF_CHIE_MASK) FTM0CH7_IRQHandler();
  if ((FTM0->SC & TOF_TOIE_MASK) == TOF_TOIE_MASK) FTM0TOF_IRQHandler();
}

void ftm0Init(bool mod_max, uint32_t modulo)
{
  // set clockgating for FTM0
  SIM->SCGC6 |= SIM_SCGC6_FTM0(1);
  FTM0->CNTIN=10;
  if(mod_max){  FTM0->MOD = 0xFFFF;}else{FTM0->MOD =modulo;}

}


void ftm0StartClk(int CLK_Source, int Prescaler)
{

  FTM0->SC = FTM_SC_CLKS(CLK_Source) |  FTM_SC_PS(Prescaler);
}

void ftm0ChangePS(int CLK_Source, int Prescaler)
{ 	ftm0StopClk();
	uint16_t val =FTM0->CNT;
	FTM0->CONTROLS[1].CnV += (UINT16_MAX-val);
	FTM0->CONTROLS[2].CnV += (UINT16_MAX-val);
	FTM0->CONTROLS[4].CnV += (UINT16_MAX-val);
	FTM0->CNT = val;		// Writing a Non-Zero Value resets the CNT to 0x0000
	FTM0->SC = FTM_SC_CLKS(CLK_Source) |  FTM_SC_PS(Prescaler);
}

void ftm0StopClk()
{
  FTM0->SC &= ~(FTM_SC_CLKS_MASK |FTM_SC_PS_MASK);
}


void ftm0EnableIRQ()
{
#if DEBUG
	Ramp_CNV_Logger[Ramp_Buffer_Index++]=FTM0->CNT;
	if (Ramp_Buffer_Index>=1000){
		Ramp_Buffer_Index =0;}
#endif
  // Enable FTM0 interrupt on NVIC with Prio: PRIO_FTM0 (defined in platform.h)
  NVIC_SetPriority(FTM0_IRQn, PRIO_FTM0);       // set interrupt priority
  NVIC_EnableIRQ(FTM0_IRQn);                    // enable interrupt
}

void ftm0StopIRQ()
{
  // Disanable FTM0 interrupt on NVIC wit)
  NVIC_DisableIRQ(FTM0_IRQn);
  FTM0->CONTROLS[0].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[3].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[5].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[6].CnSC &= ~FTM_CnSC_CHIE_MASK;
  FTM0->CONTROLS[7].CnSC &= ~FTM_CnSC_CHIE_MASK;

}


void FTM0TOF_IRQHandler(void){

	FTM0->SC &= ~FTM_SC_TOF_MASK;
}

void FTM0CH0_IRQHandler(void){
	FTM0->CONTROLS[0].CnSC &= ~FTM_CnSC_CHF(1);		//Clear TOD interrupt flag
}

void FTM0CH1_IRQHandler(void){
	FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
													// Do not manipulate!!!!!!
#if DEBUG_MODE
	RES1_GPIO_HIGH(); // Monitoring ISR-Time
#endif

#if RAMP_MODE_NSTEP 	// Ramping Part

	static bool ramp_motor_state; // true = high

if(Ramp_Step_Curr<=RAMP_NSTEPS){	// if true: Process Ramp
	if(Ramp_M1_Rem_Pending[Ramp_Step_Curr]){ // if True: Remaining Start  Ticks to Process (step_curr starts with 1)
		if(Ramp_M1_End_Rem_Ticks[Ramp_Step_Curr-1]>0){	// First Pass
			FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV+= Ramp_M1_End_Rem_Ticks[Ramp_Step_Curr];
			Ramp_M1_End_Rem_Ticks[Ramp_Step_Curr-1]=0;
		}else{
			if(Ramp_M1_End_Rem_Ticks_OF[Ramp_Step_Curr]>0){ // Initial Overflow
				// Leave ChV with the current value
				Ramp_M1_End_Rem_Ticks_OF[Ramp_Step_Curr]--;
			}else{
				Ramp_M1_Rem_Pending[Ramp_Step_Curr]=false;
				if(Ramp_M1_Start_State[Ramp_Step_Curr]==false){
					MOTOR1_STEP_GPIO_HIGH();
					FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV  += Ramp_M1_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
				else{
					MOTOR1_STEP_GPIO_LOW();
					FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV  += Ramp_M1_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
			}
		}
	}else if(ramp_motor_state){ // If State is true: 	Output is high
		if(Ramp_M1_Pulse_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M1_Pulse_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR1_STEP_GPIO_LOW();
			FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV  += Ramp_M1_Pause_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
			ramp_motor_state = true;
			Ramp_M1_Pulse_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M1_Pulse_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}else{ // If State is false: 	Output is low
		if(Ramp_M1_Pause_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M1_Pause_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR1_STEP_GPIO_HIGH();
			FTM0->CONTROLS[MOTOR1_STEP_TIMER_CHNL].CnV  += Ramp_M1_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pause
			ramp_motor_state = false;
			Ramp_M1_Pause_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M1_Pause_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}

}else{		// Ramp is done
#endif
	// If Mx_Step is false: 	Output was high before
		if (MOTOR1_STEP_STATUS()){
			MOTOR1_STEP_GPIO_LOW();									// Output was toggled to true in the current ISR
			FTM0->CONTROLS[1].CnV += (Motor1_Pause);				// Set Distance to end Pause

			// Corrector for rounding errors in calculation
			for(int i = 0;i<NUM_CORRECTOR_LOOPS;i++){
				if (Motor1_Step_Corrector[i]){
					if((Motor1_Step_Curr%Motor1_Step_Corrector[i]) == 0){
						FTM0->CONTROLS[1].CnV += 1;
					}
				}
			}

			// Disable Interrupt if all Steps are done
			if (Motor1_Step_Curr >= Motor1_Step_Max){
				FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_CHIE(1);		// Disable Interrupt when last Pulse ended
			}
		}else{				 // Output was low before: Start Pulse
#if OVERFLOW_HANDLING
			if(Motor1_Step_OF_Curr>0){
				// Leave ChV with the current value
				Motor1_Step_OF_Curr--;
			}
			else{
				MOTOR1_STEP_GPIO_HIGH();
				FTM0->CONTROLS[1].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
				Motor1_Step_Curr +=1;
				Motor1_Step_OF_Curr = Motor1_Step_OF;			// Restart Overflow counter
			}
#else
			MOTOR1_STEP_GPIO_HIGH();
			FTM0->CONTROLS[1].CnV  += MOTOR_PULSE_MOD_TICK;					// Set Distance to end Pulse
			Motor1_Step_Curr +=1;									// Increment Pulse-Counter (Beginning of Pulse)
#endif
		}

#if RAMP_MODE_NSTEP
}
#endif

#if DEBUG_MODE
		RES1_GPIO_LOW(); // Monitoring ISR-Time
#endif
}

void FTM0CH2_IRQHandler(void){
	FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
													// Do not manipulate!!!!!!
#if DEBUG_MODE
	RES1_GPIO_HIGH(); // Monitoring ISR-Time
#endif

#if RAMP_MODE_NSTEP 	// Ramping Part

	static bool ramp_motor_state; // true = high

if(Ramp_Step_Curr<=RAMP_NSTEPS){	// if true: Process Ramp
	if(Ramp_M2_Rem_Pending[Ramp_Step_Curr]){ // if True: Remaining Start  Ticks to Process (step_curr starts with 1)
		if(Ramp_M2_End_Rem_Ticks[Ramp_Step_Curr-1]>0){	// First Pass
			FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV+= Ramp_M2_End_Rem_Ticks[Ramp_Step_Curr];
			Ramp_M2_End_Rem_Ticks[Ramp_Step_Curr-1]=0;
		}else{
			if(Ramp_M2_End_Rem_Ticks_OF[Ramp_Step_Curr]>0){ // Initial Overflow
				// Leave ChV with the current value
				Ramp_M2_End_Rem_Ticks_OF[Ramp_Step_Curr]--;
			}else{
				Ramp_M2_Rem_Pending[Ramp_Step_Curr]=false;
				if(Ramp_M2_Start_State[Ramp_Step_Curr]==false){
					MOTOR2_STEP_GPIO_HIGH();
					FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV  += Ramp_M2_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
				else{
					MOTOR2_STEP_GPIO_LOW();
					FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV  += Ramp_M2_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
			}
		}
	}else if(ramp_motor_state){ // If State is true: 	Output is high
		if(Ramp_M2_Pulse_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M2_Pulse_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR2_STEP_GPIO_LOW();
			FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV  += Ramp_M2_Pause_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
			ramp_motor_state = true;
			Ramp_M2_Pulse_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M2_Pulse_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}else{ // If State is false: 	Output is low
		if(Ramp_M2_Pause_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M2_Pause_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR2_STEP_GPIO_HIGH();
			FTM0->CONTROLS[MOTOR2_STEP_TIMER_CHNL].CnV  += Ramp_M2_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pause
			ramp_motor_state = false;
			Ramp_M2_Pause_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M2_Pause_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}

}else{		// Ramp is done
#endif
	// If Mx_Step is true: 	Output was high before
		if (MOTOR2_STEP_STATUS()){
			MOTOR2_STEP_GPIO_LOW();

			FTM0->CONTROLS[2].CnV += (Motor2_Pause);	// Set Distance to end Pause
			// Corrector for rounding errors in calculation
			for(int i = 0;i<NUM_CORRECTOR_LOOPS;i++){
				if (Motor2_Step_Corrector[i]){
					if((Motor2_Step_Curr%Motor2_Step_Corrector[i]) == 0){
						FTM0->CONTROLS[2].CnV += 1;
					}
				}
			}
			// Disable Interrupt if all Steps are done
			if (Motor2_Step_Curr >= Motor2_Step_Max){
				FTM0->CONTROLS[2].CnSC &= ~FTM_CnSC_CHIE(1);		// Disable Interrupt when last Pulse ended
			}
		}else{				 // Output was low before: Start Pulse
#if OVERFLOW_HANDLING
	#if	(!(RAMP_MODE_TWOSTEP || RAMP_MODE_PREMIUM))
			if(Motor2_Step_OF_Curr>0){
				// Leave ChV with the current value
				Motor2_Step_OF_Curr--;
			}
			else{
				MOTOR2_STEP_GPIO_HIGH();
				FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
				Motor2_Step_Curr +=1;
				Motor2_Step_OF_Curr = Motor2_Step_OF;			// Restart Overflow counter
			}
	#elif	RAMP_MODE_TWOSTEP
			if(Ramp_twostep){
				if(M2_Step_Ramp_OF_Curr>0){
					// Leave ChV with the current value
					M2_Step_Ramp_OF_Curr--;
				}
				else{
					MOTOR2_STEP_GPIO_HIGH();
					FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
					Motor2_Step_Curr +=1;
					M2_Step_Ramp_OF_Curr = M2_Step_Ramp_OF;			// Restart Overflow counter
				}
			}else{
				if(Motor2_Step_OF_Curr>0){
					// Leave ChV with the current value
					Motor2_Step_OF_Curr--;
				}else{
					MOTOR2_STEP_GPIO_HIGH();
					FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
					Motor2_Step_Curr +=1;
					Motor2_Step_OF_Curr = Motor2_Step_OF;			// Restart Overflow counter
				}
			}
	#elif	RAMP_MODE_PREMIUM
			if(Ramping_Premium){
				if(M2_Step_Ramp_OF_Curr>0){
					// Leave ChV with the current value
					M2_Step_Ramp_OF_Curr--;
				}
				else{
					MOTOR2_STEP_GPIO_HIGH();
					FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
					Motor2_Step_Curr +=1;
					M2_Step_Ramp_OF_Curr = M2_Step_Ramp_OF;			// Restart Overflow counter
				}
			}else{
				if(Motor2_Step_OF_Curr>0){
					// Leave ChV with the current value
					Motor2_Step_OF_Curr--;
				}else{
					MOTOR2_STEP_GPIO_HIGH();
					FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
					Motor2_Step_Curr +=1;
					Motor2_Step_OF_Curr = Motor2_Step_OF;			// Restart Overflow counter
				}
			}
	#endif


#else
			MOTOR2_STEP_GPIO_HIGH();
			FTM0->CONTROLS[2].CnV  += MOTOR_PULSE_MOD_TICK;					// Set Distance to next Pulse
			Motor2_Step_Curr +=1;									// Increment Pulse-Counter (Beginning of Pulse)
#endif
		}

#if RAMP_MODE_NSTEP
}
#endif
#if DEBUG_MODE
		RES1_GPIO_LOW(); // Monitoring ISR-Time
#endif
}

void FTM0CH4_IRQHandler(void){
	FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
													// Do not manipulate!!!!!!
#if DEBUG_MODE
	RES1_GPIO_HIGH(); // Monitoring ISR-Time
#endif

#if RAMP_MODE_NSTEP 	// Ramping Part

	static bool ramp_motor_state; // true = high

if(Ramp_Step_Curr<=RAMP_NSTEPS){	// if true: Process Ramp
	if(Ramp_M3_Rem_Pending[Ramp_Step_Curr]){ // if True: Remaining Start  Ticks to Process (step_curr starts with 1)
		if(Ramp_M3_End_Rem_Ticks[Ramp_Step_Curr-1]>0){	// First Pass
			FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV+= Ramp_M3_End_Rem_Ticks[Ramp_Step_Curr];
			Ramp_M3_End_Rem_Ticks[Ramp_Step_Curr-1]=0;
		}else{
			if(Ramp_M3_End_Rem_Ticks_OF[Ramp_Step_Curr]>0){ // Initial Overflow
				// Leave ChV with the current value
				Ramp_M3_End_Rem_Ticks_OF[Ramp_Step_Curr]--;
			}else{
				Ramp_M3_Rem_Pending[Ramp_Step_Curr]=false;
				if(Ramp_M3_Start_State[Ramp_Step_Curr]==false){
					MOTOR3_STEP_GPIO_HIGH();
					FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV  += Ramp_M3_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
				else{
					MOTOR3_STEP_GPIO_LOW();
					FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV  += Ramp_M3_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
					ramp_motor_state=true;
				}
			}
		}
	}else if(ramp_motor_state){ // If State is true: 	Output is high
		if(Ramp_M3_Pulse_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M3_Pulse_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR3_STEP_GPIO_LOW();
			FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV  += Ramp_M3_Pause_Ticks[Ramp_Step_Curr];	// Set Distance to next Pulse
			ramp_motor_state = true;
			Ramp_M3_Pulse_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M3_Pulse_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}else{ // If State is false: 	Output is low
		if(Ramp_M3_Pause_Ticks_OF[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_M3_Pause_Ticks_OF[Ramp_Step_Curr]--;
		}else{
			MOTOR3_STEP_GPIO_HIGH();
			FTM0->CONTROLS[MOTOR3_STEP_TIMER_CHNL].CnV  += Ramp_M3_Pulse_Ticks[Ramp_Step_Curr];	// Set Distance to next Pause
			ramp_motor_state = false;
			Ramp_M3_Pause_Ticks_OF_Curr[Ramp_Step_Curr] = Ramp_M3_Pause_Ticks_OF[Ramp_Step_Curr];	// Restart Overflow counter
		}
	}

}else{		// Ramp is done
#endif
	// If Status is true: 	Output was high before
		if (MOTOR3_STEP_STATUS()){

				MOTOR3_STEP_GPIO_LOW();
				FTM0->CONTROLS[4].CnV += (Motor3_Pause);	// Set Distance to end Pause

				// Corrector for rounding errors in calculation
			for(int i = 0;i<NUM_CORRECTOR_LOOPS;i++){
				if (Motor3_Step_Corrector[i]){
					if((Motor3_Step_Curr%Motor3_Step_Corrector[i]) == 0){
						FTM0->CONTROLS[4].CnV += 1;
					}
				}
			}

			// Disable Interrupt if all Steps are done
			if (Motor3_Step_Curr >= Motor3_Step_Max){
				FTM0->CONTROLS[4].CnSC &= ~FTM_CnSC_CHIE(1);		// Disable Interrupt when last Pulse ended
			}
		}else{				 // Output was low before: Start Pulse
#if OVERFLOW_HANDLING
			if(Motor3_Step_OF_Curr>0){
				// Leave ChV with the current value
				Motor3_Step_OF_Curr--;
			}
			else{
				MOTOR3_STEP_GPIO_HIGH();
				FTM0->CONTROLS[4].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
				Motor3_Step_Curr +=1;
				Motor3_Step_OF_Curr = Motor3_Step_OF;			// Restart Overflow counter
			}
#else
				MOTOR3_STEP_GPIO_HIGH();
				FTM0->CONTROLS[4].CnV  += MOTOR_PULSE_MOD_TICK;	// Set Distance to next Pulse
				Motor3_Step_Curr +=1;							// Increment Pulse-Counter (Beginning of Pulse)
#endif
			}
#if RAMP_MODE_NSTEP
}
#endif
#if DEBUG_MODE
		RES1_GPIO_LOW(); // Monitoring ISR-Time
#endif
}
void FTM0CH5_IRQHandler(void){
    FTM0->CONTROLS[5].CnSC &= ~FTM_CnSC_CHF(1);   // Clear interrupt flag

    if (MOTORROT_STEP_STATUS()){
        // Output war HIGH: Pulse beenden, Pause starten
        MOTORROT_STEP_GPIO_LOW();
        FTM0->CONTROLS[5].CnV += MotorRot_Pause;

        // Interrupt deaktivieren wenn alle Steps fertig
        if (MotorRot_Step_Curr >= MotorRot_Step_Max){
            FTM0->CONTROLS[5].CnSC &= ~FTM_CnSC_CHIE(1);
        }
    }else{
        // Output war LOW: Pause beenden, Pulse starten
#if OVERFLOW_HANDLING
        if(MotorRot_Step_OF_Curr > 0){
            MotorRot_Step_OF_Curr--;
        }else{
            MOTORROT_STEP_GPIO_HIGH();
            FTM0->CONTROLS[5].CnV += MOTOR_PULSE_MOD_TICK;
            MotorRot_Step_Curr += 1;
            MotorRot_Step_OF_Curr = MotorRot_Step_OF;
        }
#else
        MOTORROT_STEP_GPIO_HIGH();
        FTM0->CONTROLS[5].CnV += MOTOR_PULSE_MOD_TICK;
        MotorRot_Step_Curr += 1;
#endif
    }
}

#if RAMP_MODE_NSTEP

void FTM0CH6_IRQHandler(void){
	FTM0->CONTROLS[6].CnSC &= ~FTM_CnSC_CHF(1);		// Clear TOF interrupt flag
													// Do not manipulate!!!!!!
	// INCREMENT OF CURRENT RAMP STEP

	if(Ramp_Step_Curr <= RAMP_NSTEPS){
		if(Ramp_Step_Ticks_OF_Curr[Ramp_Step_Curr]>0){
			// Leave ChV with the current value
			Ramp_Step_Ticks_OF_Curr[Ramp_Step_Curr]--;
		}else{
			FTM0->CONTROLS[6].CnV += (RAMP_NSTEPS_STEPS);	// Set Distance next step edge
			Ramp_M1_Rem_Pending[Ramp_Step_Curr]=false; // Reset Pending Flags für next Sequence
			Ramp_M2_Rem_Pending[Ramp_Step_Curr]=false;
			Ramp_M3_Rem_Pending[Ramp_Step_Curr]=false;
			Ramp_Step_Curr +=1;
		}
	}else{
		FTM0->CONTROLS[5].CnSC &= ~FTM_CnSC_CHIE(1);	// Disable Interrupt when Ramp is done
	}
}
#endif

