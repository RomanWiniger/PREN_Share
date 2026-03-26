#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define DEBUG_MODE			1
#define TODO				1


#define CLK_SRC_GLOBAL		1
#define PS_GLOBAL			4
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


//	   -----------------	                       ------
//	  | 	            | 	                      |
//    | 	            | 	                      |
//----                   -------------------------
//    <-MOTOR_PULSE_NS-> <- MIN_STEP_DISTANCE_NS->
//

// Motor

//Set Overall Parameters
#define MOTOR_PULSE_US			300		//Start Pulse to End Pulse
#define MIN_STEP_DISTANCE_US	300		//End Pulse to Start Pulse
#define FIRST_PULSE_START_MOD	1000	//Start first Pulse at this Modulo Value of the Channel

#define TIME_PER_MOD_TICK_US	((double)1/(double)TIMER_CLK_SCAL_MHZ)
#define MOTOR_PULSE_MOD_TICK	((int16_t)(MOTOR_PULSE_US/TIME_PER_MOD_TICK_US))
#define MOTOR_MINPAUSE_MOD_TICK	((int16_t)((MIN_STEP_DISTANCE_US)/TIME_PER_MOD_TICK_US))

#define NUM_CORRECTOR_LOOPS		10

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
extern int32_t Motor1_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor2_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor3_Step_OF_Curr;	// Current Amount of Overflows 0xFFFF
extern int32_t Motor1_Step_Max;
extern int32_t Motor2_Step_Max;
extern int32_t Motor3_Step_Max;
extern int32_t MotorRot_Step_Max;
extern int16_t Motor1_Pause;		//1950 = 1s
extern int16_t Motor2_Pause;		//1950 = 1s
extern int16_t Motor3_Pause; 		//1950 = 1s
extern int32_t Motor1_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor2_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp
extern int32_t Motor3_Step_Corrector[]; 	// if Current Step mod = 0: Add one Tick to Stepp


// Remember last Values for ErrorHandling
extern int32_t M1_Last_Step;
extern int32_t M2_Last_Step;
extern int32_t M3_Last_Step;
extern int32_t MR_Last_Step;
extern bool M1_Last_Dir;
extern bool M2_Last_Dir;
extern bool M3_Last_Dir;
extern bool MR_Last_Dir;

#endif
