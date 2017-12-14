/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserUSS.h
*
*   description: User defined functions tailored to HCSR04 ultrasonic distance sensor initialization and reading using
*    Tivaware source code
*
*
*
*****************************************************************************/
#ifndef DRIVERS_USERUSS_H_
#define DRIVERS_USERUSS_H_

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

/*Ultrasonic Sensor Task*/
#define MAX_TIME 7500


void USSGPIOInit(void);

#endif /* DRIVERS_USERUSS_H_ */
