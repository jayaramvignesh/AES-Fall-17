/*
 * UserTimer.h
 *
 *  Created on: Dec 13, 2017
 *      Author: Arundhathi
 */

#ifndef DRIVERS_USERTIMER_H_
#define DRIVERS_USERTIMER_H_

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


void delayMs(uint32_t ui32Ms);
void delayUs(uint32_t ui32Us);

#endif /* DRIVERS_USERTIMER_H_ */
