/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserADXL.h
*
*   description: User defined functions tailored to ADXL-GYS 345 accelerometer sensor initialization and reading using
*    Tivaware source code
*
*
*
*****************************************************************************/

#ifndef DRIVERS_USERADXL_H_
#define DRIVERS_USERADXL_H_

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
#include "main.h"
#include "utils/uartstdio.h"

#include "drivers/UserI2CLib.h"

/*ADXLSensor Task*/
/*register set definitions*/
#define DEVADDR 0x53
#define DEVID 0x00
#define PWRCTRL 0x2D
#define INTENABLE 0x2E
#define INTMAP 0x2F
#define INTSOURCE 0x30
#define DATAFORMAT 0x31
#define THRESHACT 0x24
#define THRESHINACT 0x25
#define TIMEINACT 0x26
#define ACTINACTCTL 0x27
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

void ADXLI2CInit(void);
void ADXLSInit(void);

#endif /* DRIVERS_USERADXL_H_ */
