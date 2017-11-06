/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: light.h
*
*   function prototypes
*   - light_function() function for light thread
*
*
*****************************************************************************/


#ifndef _LIGHT_H
#define _LIGHT_H

#include "main.h"
#include "message.h"
#include "sig.h"
#include "timer.h"
#include"ERROR_LIB.h"
#include"BBG_I2C2.h" 
#include"APDS9301_lightsensor.h"


void *light_function();

#endif
