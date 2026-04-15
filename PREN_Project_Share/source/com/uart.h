//         __  ___   ______   ______   __  __    _   __
//        /  |/  /  / ____/  / ____/  / / / /   / | / /
//       / /|_/ /  / /      / /_     / / / /   /  |/ /
//      / /  / /  / /___   / __/    / /_/ /   / /|  /
//     /_/  /_/   \____/  /_/       \____/   /_/ |_/
//     (c) Hochschule Luzern T&A  ==== www.hslu.ch ====
//
//     \brief   driver for the serial communication (uart)
//     \author  Christian Jost, christian.jost@hslu.ch
//     \date    24.03.2025
//     ------------------------------------------------

#ifndef SOURCES_UART_H_
#define SOURCES_UART_H_

#define NEW_LINE                '\n'




#define UART2_EN                  1     // [0|1] 1=>enable, 0=>disable
#define UART2_PRINTF_EN           0     // [0|1] redirect printf to uart 2
#define UART2_SCANF_EN            0     // [0|1] redirect scanf to uart 2
#define UART2_RX_BUF_SIZE       512     // size of the receive buffer in bytes
#define UART2_TX_BUF_SIZE       512     // size of the transmit buffer in bytes


void uart2WriteChar(char ch);
void uart2Write(const char *str);
void uart2WriteLine(const char *str);
char uart2ReadChar(void);
uint16_t uart2ReadLine(char *str, uint16_t length);
bool uart2HasLineReceived(void);
uint16_t uart2RxBufCount(void);
void uart2Init(uint32_t baudrate);


#endif /* SOURCES_UART_H_ */
