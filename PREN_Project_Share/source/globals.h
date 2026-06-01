#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define DEBUG_MODE			0
#define TEST_SEQUENCE		(0 && DEBUG_MODE) // Nur aktiv im DEBUG_MODE
#define COMMAND_BYTE		0	// @PASCAL: Zu testen -> Anpassung im Raspy-Program notwendig?
#define SENSOR_TEST			0	// @PASCAL: Zu testen -> siehe main.c
#define INIT_POS_TEST		0	// @PASCAL: Zu testen -> siehe main.c
#define SIM_SENSORS			0 	// Disable MoveToInitPosin motorinit, when no sensor is attached
#define ISR_MONITOR			0	//
#if DEBUG_MODE
#define DEBUG_MODE_SEQ		1	// Debug Channel 6 Sequence Incrementer
#define DEBUG_MODE_FTM_ST	1	// Debug Channel 4 Sequence Motor3
#define DEBUG_MODE_ISR1		1	// Debug Channel 1 Sequence Motor1
#define DEBUG_MODE_ISR2		1	// Debug Channel 2 Sequence Motor2
#define DEBUG_MODE_ISR3		1	// Debug Channel 4 Sequence Motor3
#endif
extern bool Ramp_Disabled; //flag für rampenaktivierung
//////////////////////////////////////////////////////Te
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

#define MOTOR_PULSE_US			(200)		//Start Pulse to End Pulse
#define MIN_STEP_DISTANCE_US	(200)		//End Pulse to Start Pulse
#define SLOW_MODE_FACTOR	   	(4)		//pick und place geschwindigkeit (Faktor auf die geschwindigkeit als uint16_t)
#define RAMP_DISTANCE_FACTOR	(10)		//*PREMIUM RAMP* *End Pulse to Start Pulse BEGINN LINEAR RAMP (if(<= 1) -> deactivated)
#define FIRST_PULSE_START_MOD	(1)	//Start first Pulse at this Modulo Value of the Channel

#define TIME_PER_MOD_TICK_US	((double)1/(double)TIMER_CLK_SCAL_MHZ)
#define MOTOR_PULSE_MOD_TICK	((int16_t)(MOTOR_PULSE_US/TIME_PER_MOD_TICK_US))
#define MOTOR_MINPAUSE_MOD_TICK	((int16_t)((MIN_STEP_DISTANCE_US)/TIME_PER_MOD_TICK_US))

#define NUM_CORRECTOR_LOOPS		(10)	// Rounding Error Correction

//////////////////////////////////////////////////////
// RAMP ACTIVATION
//////////////////////////////////////////////////////

#define RAMP_MODE_NSTEP		1 		// N Spedmodes: Startup and Run
#define RAMP_MODE_END		1


//////////////////////////////////////////////////////
// RAMP Config
//////////////////////////////////////////////////////
#if RAMP_MODE_NSTEP
	#define RAMP_NSTEPS				10		// NSTEP MODE: Number of Steps in Ramp (min. 2)			@Pascal: Anzahl Schritte für die Ramoe
	#define RAMP_NSTEPS_STEPS		20000	// NSTEP MODE: Number of Ticks to be ramped (per STEP)	@Pascal: Anzahl Ticks pro Schritt
	#define RAMP_NSTEPS_STEP_PERC	100		// NSTEP MODE: Inrease Time per Step [%]				@Pascal: Prozentuale Verlängerung des Schitts zum vorherigen
	#define RAMP_NSTEPS_FIRST_MOD	1000	// Ticks to set first bevore starting Modulo timer
	#define ROUNDING_ERROR_HANDLING	0		// new Todo
#endif

#if RAMP_MODE_END
	#define RAMP_END_PS1		150  // Stage 1 Reduce Prescaler at Remaining Steps
	#define RAMP_END_PS2		50  // Stage 2 Reduce Prescaler at Remaining Steps
	#define RAMP_END_PS3		0  // Stage 3 Reduce Prescaler at Remaining Steps
	#define RAMP_END_PS4		0  // Stage 4 Reduce Prescaler at Remaining Steps
#endif




//////////////////////////////////////////////////////
// GLOBAL VARIABLES (used in "motor" and "ftm"
//////////////////////////////////////////////////////

extern bool M1_Step;
extern bool M2_Step;
extern bool M3_Step;
extern int64_t Motor1_Pause_Full;
extern int64_t Motor2_Pause_Full;
extern int64_t Motor3_Pause_Full;
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

typedef struct{
	uint16_t Minimal_Pause;
	uint16_t Minimal_Pulse;
	bool RampIsDisabled;
}current_config_t;

extern current_config_t Current_Config;

extern uint16_t MotorRot_Pause; 		//1950 = 1s
extern int32_t Motor1_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor2_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor3_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp


#if RAMP_MODE_NSTEP

	extern int64_t Motor1_Pause_Full;
	extern int64_t Motor2_Pause_Full;
	extern int64_t Motor3_Pause_Full;


	extern uint16_t Ramp_M1_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1];

	extern uint16_t Ramp_Step_Curr;
	extern uint16_t Ramp_Step_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_Step_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_Step_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern bool Ramp_M1_Start_State[RAMP_NSTEPS+1];
	extern bool Ramp_M1_Rem_Pending[RAMP_NSTEPS+1]; // true if pending remainig ticks at beginning of Step (last for initial
	extern uint16_t Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_End_Rem_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pause_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pause_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pulse_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pulse_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M1_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1];

	extern bool Ramp_M2_Start_State[RAMP_NSTEPS+1];
	extern bool Ramp_M2_Rem_Pending[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_End_Rem_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pause_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pause_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pulse_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pulse_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M2_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1];

	extern bool Ramp_M3_Start_State[RAMP_NSTEPS+1];
	extern bool Ramp_M3_Rem_Pending[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_End_Rem_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_End_Rem_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pause_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pause_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pulse_Ticks[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pulse_Ticks_OF[RAMP_NSTEPS+1];
	extern uint16_t Ramp_M3_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1];

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

