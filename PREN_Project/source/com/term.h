//         __  ___   ______   ______   __  __    _   __
//        /  |/  /  / ____/  / ____/  / / / /   / | / /
//       / /|_/ /  / /      / /_     / / / /   /  |/ /
//      / /  / /  / /___   / __/    / /_/ /   / /|  /
//     /_/  /_/   \____/  /_/       \____/   /_/ |_/
//     (c) Hochschule Luzern T&A  ==== www.hslu.ch ====
//
//     \brief   terminal program
//     \author  Christian Jost, christian.jost@hslu.ch
//     \date    24.03.2025
//     ------------------------------------------------

#ifndef SOURCES_TERM_H_
#define SOURCES_TERM_H_

#include "platform.h"

typedef tError (*cmdHandler)(const char *cmd);

typedef struct commandLineHandler
{
  char cmd[16];
  char cmdDesc[32];
  cmdHandler cmdHandler;
  struct commandLineHandler *next;
} tCommandLineHandler;

struct ReceivedCommand{

	int32_t Steps1; //steps of motor1
	int32_t Steps2; //steps of motor2
	int32_t Steps3; //steps of motor3
	int32_t StepsRot; //steps of the rotation Motor
	bool ActCoil; //true if coil should be activated
	bool ErrorHandling; //in case of error go to the last position
};

void termRegisterCommandLineHandler(tCommandLineHandler *clh, char* cmd, char *cmdDesc, cmdHandler h);

void termWrite(const char *str);
void termWriteNum32s(int32_t value);
void termWriteNum16s(int16_t value);

void termWriteLine(const char *str);
void termDoWork(void);
void termInit(uint16_t baudrate);
void commandInit();


#endif /* SOURCES_TERM_H_ */
