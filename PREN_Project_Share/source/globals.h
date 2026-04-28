#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define DEBUG_MODE			0
#define TEST_SEQUENCE		(1 && DEBUG_MODE) // Nur aktiv im DEBUG_MODE
#define COMMAND_BYTE		0	// @PASCAL: Zu testen -> Anpassung im Raspy-Program notwendig?
#define SENSOR_TEST			0	// @PASCAL: Zu testen -> siehe main.c
#define INIT_POS_TEST		0	// @PASCAL: Zu testen -> siehe main.c



//////////////////////////////////////////////////////
// TIMER CONFIG
//////////////////////////////////////////////////////

#define CLK_SRC_GLOBAL		1
#define PS_GLOBAL			3
#define TIMER_MODULO_HEX	(0xFFFF)
#define TIMER_MODULO_INT	(65535)


// Clocksource and Prescaler Values
#if CLK_SRC_GLOBAL == 0
	#define TIMER_CLK		0
#elif  CLK_SRC_GLOBAL == 1
	#define TIMER_CLK		60000000
#elif  CLK_SRC_GLOBAL == 2
	#define TIMER_CLK		250000

#endif

#if PS_GLOBAL == 0
	#define TIMER_CLK_SCAL		((double)TIMER_CLK)
#elif PS_GLOBAL == 1
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/2)
#elif PS_GLOBAL == 2
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/4)
#elif PS_GLOBAL == 3
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/8)
#elif PS_GLOBAL == 4
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/16)
#elif PS_GLOBAL == 5
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/32)
#elif PS_GLOBAL == 6
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/64)
#elif PS_GLOBAL == 7
	#define TIMER_CLK_SCAL		((double)TIMER_CLK/128)
#endif

#define TIMER_CLK_SCAL_MHZ		(TIMER_CLK_SCAL/1000000)


//////////////////////////////////////////////////////
// MOTOR STEPPING CONFIGURATION FOR TIMERS
//////////////////////////////////////////////////////

//	   -----------------	                       ------
//	  | 	            | 	                      |
//    | 	            | 	                      |
//----                   -------------------------
//    <-MOTOR_PULSE_NS-> <- MIN_STEP_DISTANCE_NS->
//

// Motor

//Set Overall Parameters
#define MOTOR_PULSE_US			1000		//Start Pulse to End Pulse
#define MIN_STEP_DISTANCE_US	1000		//End Pulse to Start Pulse
#define RAMP_DISTANCE_FACTOR	10		//*PREMIUM RAMP* *End Pulse to Start Pulse BEGINN LINEAR RAMP (if(<= 1) -> deactivated)
#define FIRST_PULSE_START_MOD	1	//Start first Pulse at this Modulo Value of the Channel

#define TIME_PER_MOD_TICK_US	((double)1/(double)TIMER_CLK_SCAL_MHZ)
#define MOTOR_PULSE_MOD_TICK	((int16_t)(MOTOR_PULSE_US/TIME_PER_MOD_TICK_US))
#define MOTOR_MINPAUSE_MOD_TICK	((int16_t)((MIN_STEP_DISTANCE_US)/TIME_PER_MOD_TICK_US))

#define NUM_CORRECTOR_LOOPS		10	// Rounding Error Correction

#define RAMP_ACTIVE				1	// Startup Ramp active
#if RAMP_ACTIVE
	#define RAMP_MODE_PS		1						// Prescaler modulation in 4 Steps
	#define RAMP_MODE_NSTEP		0 && !RAMP_MODE_PS		// N Spedmodes: Startup and Run
	#define RAMP_MODE_PREMIUM	0 && !RAMP_MODE_TWOSTEP	// Calculation in ISR, Linear
	#define RAMP_NUMB_STEPS		50		//Fastest Motor: Ramp from start to this step number

	#define RAMP_DIV1			20	// PRESCALER MODE: Divider for Stepnumber for 1. Ramp part
	#define RAMP_DIV2			10	// PRESCALER MODE: Divider for Stepnumber for 2. Ramp part
	#define RAMP_DIV3			2	// PRESCALER MODE: Divider for Stepnumber for 3. Ramp part

    #define RAMP_NSTEPS				5		// NSTEP MODE: Number of Steps in Ramp (min. 2)
    #define RAMP_NSTEPS_STEPS		200		// NSTEP MODE: Number of Ticks to be ramped (per STEP)
    #define RAMP_NSTEPS_STEP_TICKS	20		// NSTEP MODE: Inrease Time per Step [%]
    #define RAMP_NSTEPS_STEP_VAR	0		// Unused

#endif

#if RAMP_MODE_PREMIUM
	#define ADD_TICK_PER_STEP	(((RAMP_DISTANCE_FACTOR-1)*MOTOR_MINPAUSE_MOD_TICK)/RAMP_NUMB_STEPS)
#endif


//////////////////////////////////////////////////////
// GLOBAL VARIABLES (used in "motor" and "ftm"
//////////////////////////////////////////////////////

extern bool M1_Step;
extern bool M2_Step;
extern bool M3_Step;
extern int32_t Motor1_Step_Curr;
extern int32_t Motor2_Step_Curr;
extern int32_t Motor3_Step_Curr;
extern int32_t MotorRot_Step_Curr;
extern int32_t Motor1_Step_OF;	// Amount of Overflows 0xFFFF
extern int32_t Motor2_Step_OF;	// Amount of Overflows 0xFFFF
extern int32_t Motor3_Step_OF;	// Amount of Overflows 0xFFFF
extern int32_t MotorRot_Step_OF;	// Amount of Overflows 0xFFFF
extern int32_t Motor1_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor2_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor3_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t MotorRot_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor1_Step_Max;
extern int32_t Motor2_Step_Max;
extern int32_t Motor3_Step_Max;
extern int32_t MotorRot_Step_Max;
extern uint16_t Motor1_Pause;
extern uint16_t Motor2_Pause;
extern uint16_t Motor3_Pause;
extern uint16_t M1_Pause_min;
extern uint16_t M2_Pause_min;
extern uint16_t M3_Pause_min;
extern uint16_t M1_Pause_Ramp;
extern uint16_t M2_Pause_Ramp;
extern uint16_t M3_Pause_Ramp;
extern double Ramp_Factor_current;

extern uint16_t MotorRot_Pause; 		//1950 = 1s
extern int32_t Motor1_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor2_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor3_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp

#if RAMP_MODE_TWOSTEP
	extern bool Ramp_twostep;
	extern int32_t M1_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	extern int32_t M2_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	extern int32_t M3_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	extern int32_t M1_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
	extern int32_t M2_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
	extern int32_t M3_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
#endif

#if RAMP_MODE_PREMIUM
	extern bool Ramping_Premium;
	extern int64_t M1_PR_Ramp_TCK_Add; // Ticks to Add per Step WHILE RAMPING
	extern int64_t M2_PR_Ramp_TCK_Add; // Ticks to Add per Step WHILE RAMPING
	extern int64_t M3_PR_Ramp_TCK_Add; // Ticks to Add per Step WHILE RAMPING
	extern int16_t M1_PR_Ramp_Pause[]; // Pause Ticks per Step
	extern int16_t M2_PR_Ramp_Pause[]; // Pause Ticks per Step
	extern int16_t M3_PR_Ramp_Pause[]; // Pause Ticks per Step
	extern int16_t M1_PR_Ramp_OF[]; // Overflow Count
	extern int16_t M2_PR_Ramp_OF[]; // Overflow Count
	extern int16_t M3_PR_Ramp_OF[]; // Overflow Count
	extern int16_t M1_PR_Ramp_OF_Curr; // Overflow Count Current
	extern int16_t M2_PR_Ramp_OF_Curr; // Overflow Count Current
	extern int16_t M3_PR_Ramp_OF_Curr; // Overflow Count Current
#endif

#if RAMP_MODE_NSTEP
	extern int64_t Ramp_Ticks[RAMP_NSTEPS];		// Array wirh Ticks per Ramp Step
	extern bool Ramp_M1_Start_State[RAMP_NSTEPS];			//true = Pulse false )
	extern bool Ramp_M1_End_State[RAMP_NSTEPS];			//true = Pulse false )
	extern int64_t Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pause_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M1_Pulse_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )

	extern bool Ramp_M2_Start_State[RAMP_NSTEPS];			//true = Pulse false )
	extern bool Ramp_M2_End_State[RAMP_NSTEPS];			//true = Pulse false )
	extern int64_t Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pause_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M2_Pulse_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )

	extern bool Ramp_M3_Start_State[RAMP_NSTEPS];			//true = Pulse false )
	extern bool Ramp_M3_End_State[RAMP_NSTEPS];			//true = Pulse false )
	extern int64_t Ramp_M3_End_Rem_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pause_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks_Init[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks_Init_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks_Init_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks_OF[RAMP_NSTEPS];			//true = Pulse false )
	extern uint16_t Ramp_M3_Pulse_Ticks_OF_Curr[RAMP_NSTEPS];			//true = Pulse false )

#endif

#if DEBUG
	extern uint16_t Ramp_CNV_Logger[];
	extern uint16_t Ramp_Buffer_Index;
#endif

// Remember last Values for ErrorHandling
extern int32_t M1_Last_Step;
extern int32_t M2_Last_Step;
extern int32_t M3_Last_Step;
extern int32_t MR_Last_Step;
extern bool M1_Last_Dir;
extern bool M2_Last_Dir;
extern bool M3_Last_Dir;
extern bool MR_Last_Dir;





#endif // END OF GLOBALS_H

