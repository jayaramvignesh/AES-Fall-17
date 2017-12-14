/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: UserI2CLib.h
*
*   description: User defined functions tailored to current project using I2C driver lib from Tivaware source code
*
*
*****************************************************************************/


#ifndef USERI2CLIB_H_
#define USERI2CLIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/i2c.h"
#include "driverlib/i2c.h"
#include "drivers/pinout.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"


void getI2CNo(uint32_t no);
uint32_t readReg(uint32_t devaddr, uint32_t reg);
uint32_t writeReg(uint32_t devaddr, uint32_t reg, uint32_t value);


#endif /* USERI2CLIB_H_ */
