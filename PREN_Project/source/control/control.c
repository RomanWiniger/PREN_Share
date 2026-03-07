#include "control.h"
#include "platform.h"
#include "term.h"
#include "motor.h"

void controlInit(){

	motInit();
}

void newCommand(struct ReceivedCommand command)//therm.c calls this function if a new command was sent

{

	//hier erfolgt die vertielung der befehle auf die einzelnen komponenten
}
