/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 14th Dec 2017
*
*   file: sig.h
*
*   function prototypes
*   - signal_handler() function for handling signal SIGINT
*   - setup_SIGINT() function to setup the signal sIGINT
*
*
*****************************************************************************/



#ifndef _SIG_H
#define _SIG_H

#include "main.h"

void signal_handler();
void setup_SIGINT();

#endif


