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

#include "platform.h"
#include "term.h"
#include "uart.h"
#include "string.h"
#include "util.h"
#include "control.h"
#include <stdlib.h>


typedef enum
{
  uart0 = 1,
  uart2 = 2,
  lpuart0 = 4,
  uartAll = (uart0 | uart2 | lpuart0)
} tUart;

static tCommandLineHandler start; //declaration of the handler for commands from The Raspberry
static tCommandLineHandler initCmd;
static tCommandLineHandler cameraCmd;
static tCommandLineHandler puzzleBeginCmd;
static tCommandLineHandler puzzleEndCmd;



static tUart destination;
static tCommandLineHandler *head = NULL;

void termRegisterCommandLineHandler(tCommandLineHandler *clh, char* cmd, char *cmdDesc, cmdHandler h)
{
  clh->next = head;
  head = clh;

  strncpy(clh->cmd, cmd, sizeof(clh->cmd));
  strncpy(clh->cmdDesc, cmdDesc, sizeof(clh->cmdDesc));
  clh->cmdHandler = h;
}

/**
 * Writes a character to the uart
 * The output is sent to the uart which received the last command
 *
 * @param[in] ch
 *   the character to send
 */
void termWriteChar(char ch)
{
    // check if uart2 is enabled
    #if (UART2_EN)
      if (destination & uart2) uart2WriteChar(ch);
    #endif
  }

/**
 * Writes a null terminated string in the send buffer. If the
 * string is null, the function returns immediately.
 *
 * @param[in] str
 *   the null terminated string to send
 */
void termWrite(const char *str)
{
  if (str == NULL) return;
  while (*str != '\0') termWriteChar(*str++);
}

void termWriteNum32s(int32_t value)
{
  char str[sizeof("-2147483648")];
  utilNum32sToStr(str, sizeof(str), value);
  termWrite(str);
}

void termWriteNum16s(int16_t value)
{
  char str[sizeof("-32768")];
  utilNum16sToStr(str, sizeof(str), value);
  termWrite(str);
}

/**
 * Writes a null terminated string in the send buffer. If the
 * string is null, only a new new line character is sent.
 *
 * @param[in] str
 *   the null terminated string to send
 */
void termWriteLine(const char *str)
{
  termWrite(str);
  termWriteChar(NEW_LINE);
}

/**
 * Writes a substring in the send buffer.
 *
 * @param[in] str
 *   the null terminated string to send
 * @param[in] start
 *   the start position of the substring to send
 * @param[in] length
 *   the number of characters to send
 */
void termWriteSubString(char *str, uint8_t start, uint8_t length)
{
  str += start;
  while(length--) termWriteChar(*str++);
}

/**
 * Parses one command line and executes the appropriate command.
 *
 * @param[in] cmd
 *   the null terminated string to process
 */
void termParseCommand(char *cmd)
{
  uint8_t subCmdLength = 0;
  tError result = EC_INVALID_CMD;
  tCommandLineHandler *clh = head;
  char buf[16];


  if (strcmp(cmd, "help") == 0)
  {
    termWriteLine(NULL);
    termWriteLine(">>> MC-Car v2 terminal ready <<<");
    termWriteLine("valid commands are:");
    while(clh != NULL)
    {
      termWrite("  ");
      termWrite(clh->cmd);
      termWrite("-> ");
      termWriteLine(clh->cmdDesc);
      clh = clh->next;
    }
    result = EC_SUCCESS;
  }

  else
  {
    while(clh != NULL)
    {
      if (strncmp(cmd, clh->cmd, strlen(clh->cmd)) == 0)
      {
        subCmdLength = strlen(clh->cmd);
        result = clh->cmdHandler(cmd + strlen(clh->cmd));
        break;
      }
      clh = clh->next;
    }
  }

  if (result != EC_SUCCESS)
  {
    termWriteLine(NULL);
    termWrite(cmd);
    termWriteLine(": command not found!");
    termWrite("Error Code: ");
    buf[0] = '\0';
    utilNum16uToStr(buf, sizeof(buf), result);
    termWriteLine(buf);
    termWrite("Enter '");
    termWriteSubString(cmd, 0, subCmdLength);
    termWriteLine("help' for a list of valid commands.");
  }
}

/**
 * @brief Terminal do work
 *
 * This function reads a line from the uart and calls the
 * termParseCommand function to process the command.
 */
void termDoWork(void)
{
  char cmd[512];


    #if (UART2_EN)
      if (uart2HasLineReceived())                   // process uart data from debug interface (UART2)
      {
        destination = uart2;
        uart2ReadLine(cmd, sizeof(cmd));
        termParseCommand(cmd);

    #endif
  }
}

/**
 * Initialize the terminal with the desired baudrate.
 *
 * @param[in] baudrate
 *   the desired baudrate (for example: 9600, 19200, 57600...)
 */
void termInit(uint32_t baudrate)
{

    // initialize uart2 only if the uart is enabled
    #if (UART2_EN)
      uart2Init(baudrate);
    #endif
    termWriteLine(NULL);
    termWriteLine("tinyk22 ready... :-)");
 }

static tError startHandler(const char *args)
{
	struct ReceivedCommand commands;
	char* current;

#if COMMAND_BYTE
	commands.CMD[0] = (bool)strtol(args, &current, 10);//strtol extracts the values
	commands.Steps1 = strtol(current, &current, 10);//strtol extracts the values
#else
	commands.Steps1 = strtol(args, &current, 10);//strtol extracts the values
#endif
	commands.Steps2 = strtol(current, &current, 10);
	commands.Steps3 = strtol(current, &current, 10);
	commands.StepsRot = strtol(current, &current, 10);
	commands.ActCoil = (bool)strtol(current, &current, 10);
	commands.ErrorHandling = (bool)strtol(current, &current, 10);

	newCommand(commands);

	return EC_SUCCESS;
}

static tError initHandler(const char *args)
{
	moveToInitPos(1000);
    termWriteLine("OK");
    return EC_SUCCESS;
}

static tError cameraHandler(const char *args)
{
    //Roboter in Kameraposition fahren
	moveWay(1000, 1000, 1000);//to be defined
    termWriteLine("OK");
    return EC_SUCCESS;
}

static tError puzzleBeginHandler(const char *args)
{
	MoveToInitPos(1000);
    return EC_SUCCESS;
}

static tError puzzleEndHandler(const char *args)
{
    termWriteLine("FINISH");
    return EC_SUCCESS;
}




void commandInit(){

	 termRegisterCommandLineHandler(
	 &start,//storrage location
	 "start ",//name of the command
	 "start command",//description
	 startHandler//the handler of the command
	    );

	 termRegisterCommandLineHandler(
	 &initCmd, "INIT", "Initialize robot", initHandler);

	 termRegisterCommandLineHandler(
	 &cameraCmd, "CAMERA", "Move to camera position", cameraHandler);

	 termRegisterCommandLineHandler(
	 &puzzleBeginCmd, "PUZZLE_BEGIN", "Begin puzzle sequence", puzzleBeginHandler);

	 termRegisterCommandLineHandler(
	 &puzzleEndCmd, "PUZZLE_END", "End puzzle sequence", puzzleEndHandler);

}

