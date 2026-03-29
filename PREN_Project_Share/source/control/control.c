#include "control.h"
#include "platform.h"
#include "term.h"
#include "motor.h"
#include "coil.h"
#include "wait.h"
#include <Stdbool.h>
#include "globals.h"

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
	motorInit();	// Pins for Stepper Motors
	coilInit();		// Pins for Coil

}

void newCommand(struct ReceivedCommand command)//therm.c calls this function if a new command was sent
{
	//////////////////////////////////////////////////////////////////
	///  COIL ACTIVATION
	//////////////////////////////////////////////////////////////////
	if(command.ActCoil){coil_ctrl(true);}

	//////////////////////////////////////////////////////////////////
	///  MOVE AND ROTATE
	//////////////////////////////////////////////////////////////////
	if (((command.Steps1 > 0)||(command.Steps2 > 0)||(command.Steps3 > 0))&&(command.ErrorHandling==false)){
		moveWay(command.Steps1,command.Steps2,command.Steps3);
	}else if ((command.StepsRot > 0)&&(command.ErrorHandling==false)){
		moveRotation(command.StepsRot);
	}else if (command.ErrorHandling==true){
		// Use last saved Values but with contrary direction
		moveWay(M1_Last_Step,M2_Last_Step,M3_Last_Step);
		moveRotation(MR_Last_Step);
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
