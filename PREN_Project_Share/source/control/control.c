#include "control.h"
#include "platform.h"
#include "term.h"
#include "motor.h"
#include "wait.h"
#include <Stdbool.h>

int32_t Motor1_Step_Curr=0;
int32_t Motor2_Step_Curr=0;
int32_t Motor3_Step_Curr=0;
int32_t Motor1_Step_Max=0;//1950 = 1s
int32_t Motor2_Step_Max=0; //1950 = 1s
int32_t Motor3_Step_Max=0; //1950 = 1s
int16_t Motor1_Pause = 0;
int16_t Motor2_Pause = 0;
int16_t Motor3_Pause = 0;
int32_t M1_Last_Step=0;//1950 = 1s
int32_t M2_Last_Step=0; //1950 = 1s
int32_t M3_Last_Step=0; //1950 = 1s
bool M1_Last_Dir=false;			// Corresponds to the Output on the FTM Timer
bool M2_Last_Dir=false;
bool M3_Last_Dir=false;

bool M1_Step=false;			// Corresponds to the Output on the FTM Timer
bool M2_Step=false;
bool M3_Step=false;

void controlInit(){

	waitInit();
	motorInit();

}

void newCommand(struct ReceivedCommand command)//therm.c calls this function if a new command was sent
{
	//TODO: Activate Coil Pin if Act.Coil == true;



	if ((command.StepsRot <= 0)&&(command.ErrorHandling==false)){
		moveWay(command.Steps1,command.Steps2,command.Steps3);
	}else if ((command.StepsRot > 0)&&(command.ErrorHandling==false)){
		moveRotation(command.StepsRot);
	}else if (command.ErrorHandling==true){
		// Use last saved Values but with contrary direction
		moveWay(M1_Last_Step,M2_Last_Step,M3_Last_Step);;
	}
}
