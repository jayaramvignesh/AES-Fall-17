/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: logger.h
*
*   function prototypes
*   - logger_function() function for logger thread
*
*
*****************************************************************************/



#ifndef _LOGGER_H
#define _LOGGER_H

#include "main.h"
#include "message.h"
#include "sig.h"
#include "timer.h"

void *logger_function();

#endif
