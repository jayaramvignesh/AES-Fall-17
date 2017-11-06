/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: temperature.h
*
*   function prototypes
*   - temperature_function() function for temperature thread
*
*
*****************************************************************************/


#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "main.h"
#include "message.h"
#include "sig.h"
#include "timer.h"
#include"ERROR_LIB.h"
#include"BBG_I2C2.h"
#include"TMP102_temperaturesensor.h"


void *temperature_function();

#endif
