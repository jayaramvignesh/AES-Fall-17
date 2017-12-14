/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: decision.h
*
*   function prototypes
*   - decision_function() function for decision thread
*
*
*****************************************************************************/



#ifndef _BBG_UART_H
#define _BBG_UART_H

#include "main.h"
#include "sig.h"
#include "timer.h"

struct termios *configure;
void tty_config(struct termios *con, int descriptor);
int fd;

void tty_config(struct termios *con, int descriptor);
void uart_init(void);

#endif
