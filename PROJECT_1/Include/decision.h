/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: decision.h
*
*   function prototypes
*   - decision_function() function for decision thread
*
*
*****************************************************************************/



#ifndef _DECISION_H
#define _DECISION_H

#include "main.h"
#include "message.h"
#include "sig.h"
#include "timer.h"
#include"BBG_LED.h"
#include"ERROR_LIB.h"

void *decision_function();

#endif
