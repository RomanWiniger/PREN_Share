#include "globals.h"
#include "control.h"
#include "platform.h"
#include "term.h"
#include "motor.h"
#include "coil.h"
#include "sensor.h"
#include "wait.h"
#include <Stdbool.h>

void clkGating(void); // GPIO Ports A...E


//////////////////////////////////////////////////////
// Initialize Global Variables
//////////////////////////////////////////////////////
int32_t Motor1_Step_Curr=0;
int32_t Motor2_Step_Curr=0;
int32_t Motor3_Step_Curr=0;
int32_t MotorRot_Step_Curr=0;
int32_t Motor1_Step_Max=0;//1950 = 1s
int32_t Motor2_Step_Max=0; //1950 = 1s
int32_t Motor3_Step_Max=0; //1950 = 1s
int32_t MotorRot_Step_Max=0; //1950 = 1s
int32_t Motor1_Step_OF=0;	// Amount of Overflows 0xFFFF
int32_t Motor2_Step_OF=0;	// Amount of Overflows 0xFFFF
int32_t Motor3_Step_OF=0;	// Amount of Overflows 0xFFFF
int32_t MotorRot_Step_OF=0;	// Amount of Overflows 0xFFFF
int32_t Motor1_Step_OF_Curr=0;	// Current Amount of Overflows 0xFFFF
int32_t Motor2_Step_OF_Curr=0;	// Current Amount of Overflows 0xFFFF
int32_t Motor3_Step_OF_Curr=0;	// Current Amount of Overflows 0xFFFF
int32_t MotorRot_Step_OF_Curr=0;	// Current Amount of Overflows 0xFFFF
uint16_t Motor1_Pause = 0;
uint16_t Motor2_Pause = 0;
uint16_t Motor3_Pause = 0;
uint16_t M1_Pause_min=0;
uint16_t M2_Pause_min=0;
uint16_t M3_Pause_min=0;
uint16_t M1_Pause_Ramp=0;
uint16_t M2_Pause_Ramp=0;
uint16_t M3_Pause_Ramp=0;
double Ramp_Factor_current =1;

#if RAMP_MODE_TWOSTEP
	bool Ramp_twostep = false;
	int32_t M1_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	int32_t M2_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	int32_t M3_Step_Ramp_OF;	// Amount of Overflows 0xFFFF
	int32_t M1_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
	int32_t M2_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
	int32_t M3_Step_Ramp_OF_Curr;	// Current Amount of Overflows 0xFFFF
#endif

#if RAMP_MODE_PREMIUM
	bool Ramping_Premium = false;
	int64_t M1_PR_Ramp_TCK_Add=0; // Ticks to Add per Step WHILE RAMPING
	int64_t M2_PR_Ramp_TCK_Add=0; // Ticks to Add per Step WHILE RAMPING
	int64_t M3_PR_Ramp_TCK_Add=0; // Ticks to Add per Step WHILE RAMPING
	int16_t M1_PR_Ramp_Pause[RAMP_NUMB_STEPS]={0}; // Pause Ticks per Step
	int16_t M2_PR_Ramp_Pause[RAMP_NUMB_STEPS]={0}; // Pause Ticks per Step
	int16_t M3_PR_Ramp_Pause[RAMP_NUMB_STEPS]={0}; // Pause Ticks per Step
	int16_t M1_PR_Ramp_OF[RAMP_NUMB_STEPS]={0}; // Overflow Count
	int16_t M2_PR_Ramp_OF[RAMP_NUMB_STEPS]={0}; // Overflow Count
	int16_t M3_PR_Ramp_OF[RAMP_NUMB_STEPS]={0}; // Overflow Count
	int16_t M1_PR_Ramp_OF_Curr=0; // Overflow Count Current
	int16_t M2_PR_Ramp_OF_Curr=0; // Overflow Count Current
	int16_t M3_PR_Ramp_OF_Curr=0; // Overflow Count Current
#endif

#if RAMP_MODE_NSTEP
	int64_t Motor1_Pause_Full=0;
	int64_t Motor2_Pause_Full=0;
	int64_t Motor3_Pause_Full=0;

	uint16_t Ramp_Step_Curr=0;	// first ISR sets to step
	uint16_t Ramp_Step_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_Step_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_Step_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};

	bool Ramp_M1_Start_State[RAMP_NSTEPS+1]={false};
	bool Ramp_M1_Rem_Pending[RAMP_NSTEPS+1]={false};
	uint16_t Ramp_M1_End_Rem_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_End_Rem_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pause_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pause_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pulse_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pulse_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M1_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};

	bool Ramp_M2_Start_State[RAMP_NSTEPS+1]={false};
	bool Ramp_M2_Rem_Pending[RAMP_NSTEPS+1]={false};
	uint16_t Ramp_M2_End_Rem_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_End_Rem_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pause_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pause_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pulse_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pulse_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M2_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};

	bool Ramp_M3_Start_State[RAMP_NSTEPS+1]={false};
	bool Ramp_M3_Rem_Pending[RAMP_NSTEPS+1]={false};
	uint16_t Ramp_M3_End_Rem_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_End_Rem_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_End_Rem_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pause_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pause_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pause_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pulse_Ticks[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pulse_Ticks_OF[RAMP_NSTEPS+1]={0};
	uint16_t Ramp_M3_Pulse_Ticks_OF_Curr[RAMP_NSTEPS+1]={0};

#endif

#if DEBUG
	uint16_t Ramp_CNV_Logger[1000];
	uint16_t Ramp_Buffer_Index=0;
#endif

uint16_t MotorRot_Pause = 0;
int32_t Motor1_Step_Corrector[NUM_CORRECTOR_LOOPS]={0}; 	// if Current Step mod = 0: Add one Tick to Stepp
int32_t Motor2_Step_Corrector[NUM_CORRECTOR_LOOPS]={0}; 	// if Current Step mod = 0: Add one Tick to Stepp
int32_t Motor3_Step_Corrector[NUM_CORRECTOR_LOOPS]={0}; 	// if Current Step mod = 0: Add one Tick to Stepp
int32_t M1_Last_Step=0;//1950 = 1s
int32_t M2_Last_Step=0; //1950 = 1s
int32_t M3_Last_Step=0; //1950 = 1s
int32_t MR_Last_Step=0; //1950 = 1s
bool M1_Last_Dir=false;			// Corresponds to the Output on the FTM Timer
bool M2_Last_Dir=false;
bool M3_Last_Dir=false;
bool MR_Last_Dir=false;
bool M1_Step=false;			// Corresponds to the Output on the FTM Timer
bool M2_Step=false;
bool M3_Step=false;
bool MR_Step=false;


void controlInit(){

	waitInit();		// Init for waitMs()-Function
	clkGating();	// GPIO Ports A...E
	sensorInit();		// Pins for Coil
	motorInit();	// Pins for Stepper Motors
	coilInit();		// Pins for Coil


}

void newCommand(struct ReceivedCommand command)//therm.c calls this function if a new command was sent
{
	//////////////////////////////////////////////////////////////////
	///  COIL ACTIVATION
	//////////////////////////////////////////////////////////////////
	if(command.ActCoil){coil_ctrl(true);}
	else(coil_ctrl(false));

	//////////////////////////////////////////////////////////////////
	///  MOVE AND ROTATE
	//////////////////////////////////////////////////////////////////
#if COMMAND_BYTE
	if(command.CMD[0]){moveToInitPos();}
#endif

	if (((command.Steps1 != 0)||(command.Steps2 != 0)||(command.Steps3 != 0))&&(command.ErrorHandling==false)){
		moveWay(command.Steps1,command.Steps2,command.Steps3);
	}if ((command.StepsRot > 0)&&(command.ErrorHandling==false)){
		moveRotation(command.StepsRot);
	}else if (command.ErrorHandling==true){
		// Use last saved Values but with contrary direction
		moveWay(M1_Last_Step,M2_Last_Step,M3_Last_Step);
#if ENABLE_ROT
		moveRotation(MR_Last_Step);
#endif
	}
}


void clkGating(void){
	//////////////////////////////////////////////////////
	// Pin Muxing / Clockgateing GPIO
	//////////////////////////////////////////////////////

	 SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
	 SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
}
