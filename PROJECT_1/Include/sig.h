/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
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
#include "message.h"
#include "timer.h"

/*set up handler for control+C*/
sigset_t sigint_mask;
struct sigaction sigint_handler;
 
void signal_handler();
void setup_SIGINT();

#endif


