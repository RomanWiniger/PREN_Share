/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Common settings of the FTM0
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          04.04.2020
 *
 *--------------------------------------------------------------------
 */

#ifndef SOURCES_FTM0_H_
#define SOURCES_FTM0_H_

void ftm0Init(bool mod_max, uint32_t modulo);
void ftm0StartClk(int CLK_Source, int Prescaler);
void ftm0ChangePS(int CLK_Source, int Prescaler);
void ftm0StopClk(void);
void ftm0EnableIRQ();
void ftm0StopIRQ();

#endif /* SOURCES_FTM0_H_ */
