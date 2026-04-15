
#include "platform.h"
#include "uart.h"

#if UART2_EN && !SOLUTION

/**
 * the receive queue of this driver, implemented as ring buffer
 */
static char rxBuf[UART2_RX_BUF_SIZE];
static volatile uint16_t rxBufCount;
static uint16_t rxBufWritePos;
static uint16_t rxBufReadPos;

/**
 * the transmit queue of this driver, implemented as ring buffer
 */
static char txBuf[UART2_TX_BUF_SIZE];
static volatile uint16_t txBufCount;
static uint16_t txBufWritePos;
static uint16_t txBufReadPos;

/**
 * @brief UART Interrupt Service Routine
 * - Received bytes are stored in the queue rxBuf
 * - Bytes in the queue txBuf are sent
 */
void UART2_RX_TX_IRQHandler(void)
{
  OnEnterUart2RxTxISR();

  uint8_t status = UART2->S1;

  if (status & UART_S1_RDRF_MASK)
  {
    uint8_t data = UART2->D;
    if (rxBufCount < UART2_RX_BUF_SIZE)
    {
      rxBuf[rxBufWritePos++] = (char)data;
      if (rxBufWritePos == UART2_RX_BUF_SIZE) rxBufWritePos = 0;
      rxBufCount++;
    }
  }

  if (status & UART_S1_TDRE_MASK)
  {
    if (txBufCount > 0)
    {
      UART2->D = txBuf[txBufReadPos++];
      txBufCount--;
      if (txBufReadPos == UART2_TX_BUF_SIZE) txBufReadPos = 0;
    }
    else
    {
      UART2->C2 &= ~UART_C2_TIE_MASK;
    }
  }

  OnExitUart2RxTxISR();
}

/**
 * Error Interrupt Service Routine
 * Clears the error flags.
 */
void UART2_ERR_IRQHandler(void)
{
  (void)UART2->S1;
  (void)UART2->D;
}

/**
 * Writes one Byte in the transmit buffer.
 *
 * @details
 *   Switching on the TIE interrupt causes an interrupt to be
 *   triggered immediately. The function blocks until there is
 *   space in the txBuf queue.
 *
 * @param[in] ch
 *   the byte to send
 */
void uart2WriteChar(char ch)
{
  while (txBufCount >= UART2_TX_BUF_SIZE);
  txBuf[txBufWritePos++] = ch;
  if (txBufWritePos == UART2_TX_BUF_SIZE) txBufWritePos = 0;

  NVIC_DisableIRQEx(UART2_RX_TX_IRQn);
  txBufCount++;
  NVIC_EnableIRQ(UART2_RX_TX_IRQn);

  UART2->C2 |= UART_C2_TIE_MASK;
}

/**
 * Writes a null terminated string in the send buffer. If the
 * string is null, the function returns immediately.
 *
 * @param[in] str
 *   the null terminated string to send
 */
void uart2Write(const char *str)
{
  if (str == NULL) return;
  while (*str != '\0') uart2WriteChar(*str++);
}

/**
 * Writes a null terminated string in the send buffer. If the
 * string is null, only a new line character is sent.
 *
 * @param[in] str
 *   the null terminated string to send
 */
void uart2WriteLine(const char *str)
{
  uart2Write(str);
  uart2WriteChar(NEW_LINE);
}

/**
 * Reads one char out of the rxBuf queue. The function blocks
 * until there is at least one byte in the queue.
 *
 * @return
 *   the received byte
 */
char uart2ReadChar(void)
{
  char ch;
  while (rxBufCount == 0);

  ch = rxBuf[rxBufReadPos++];
  if (rxBufReadPos == UART2_RX_BUF_SIZE) rxBufReadPos = 0;

  NVIC_DisableIRQEx(UART2_RX_TX_IRQn);
  rxBufCount--;
  NVIC_EnableIRQ(UART2_RX_TX_IRQn);

  return ch;
}

/**
 * Reads a null terminated string out of the rxBuf queue. The
 * function blocks until a new line character has been received
 * or the length has been exceeded.
 *
 * @details
 *   the new line character will be replaced with a '\0' to
 *   terminate the string.
 *
 * @param[out] *str
 *   pointer to a char array to store the received string
 * @param[in] length
 *   the length of the str char array.
 * @returns
 *   the length of the received string.
 */
uint16_t uart2ReadLine(char *str, uint16_t length)
{
  uint16_t i;
  for (i = 1; i < length; i++)
  {
    *str = uart2ReadChar();
    if (*str == NEW_LINE)
    {
      *str = '\0';
      break;
    }
    str++;
  }
  return i;
}

/**
 * This functions checks, if there is a new line character
 * in the rxBuf queue.
 *
 * @returns
 *   TRUE, if there is a new line character, otherweise FALSE.
 */
bool uart2HasLineReceived(void)
{
  uint16_t i;
  uint16_t index = rxBufReadPos;

  for (i = 0; i < rxBufCount; i++)
  {
    if (rxBuf[index++] == NEW_LINE) return TRUE;
    if (index == UART2_RX_BUF_SIZE) index = 0;
  }
  return FALSE;
}

/**
 * Returns the number of bytes in the receiver queue.
 *
 * @returns
 *   the number of bytes in the receiver queue.
 */
uint16_t uart2RxBufCount(void)
{
  return rxBufCount;
}

#if UART2_PRINTF_EN
/**
 * Redirects printf output to uart2
 */
int _write(int fd, const char *buf, int count)
{
  int i = count;
  while (i--) uart2WriteChar(*(uint8_t*)buf++);
  return count;
}
#endif

#if UART2_SCANF_EN
/**
 * Redirects scanf input to uart2
 */
int _read(int fd, char *buf, int count)
{
  *buf = uart2ReadChar();
  return 1;
}
#endif

void uart2Init(uint32_t baudrate)
{
  txBufReadPos = txBufWritePos = txBufCount = 0;
  rxBufReadPos = rxBufWritePos = rxBufCount = 0;

  //Clock Gating
    SIM ->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM ->SCGC5 |= SIM_SCGC5_PORTD_MASK;


   //Pins der UART initialisieren
    PORTD ->PCR[2] = (1<<9)|(1<<8)|(1<<1)|(1);
    PORTD ->PCR[3] = (1<<9)|(1<<8)|(1<<1)|(1);


    //Die Baudrate in einen Teilungsfaktor umrechnen und in das Register schreiben
    int Teilungsfaktor = BUSCLOCK/(16*baudrate);
    UART2->BDH = (Teilungsfaktor>>8&0x1F);//die oberen bits schreiben
    UART2->BDL = (Teilungsfaktor&0xFF);//die unteren bits setzen


    //Transmitter und Receiver enablen, Interrrupts aktivieren
    UART2->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK;


  // enable the error interrupts of the uart and configure the NVIC
  UART2->C3 = UART_C3_ORIE_MASK | UART_C3_NEIE_MASK | UART_C3_FEIE_MASK;

  NVIC_SetPriority(UART2_RX_TX_IRQn, PRIO_UART2);
  NVIC_EnableIRQ(UART2_RX_TX_IRQn);

  NVIC_SetPriority(UART2_ERR_IRQn, PRIO_UART2);
  NVIC_EnableIRQ(UART2_ERR_IRQn);
}
#endif// UART2_EN && !SOLUTION
