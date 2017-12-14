/* FreeRTOS 8.2 Tiva Demo
 *
 * I2C_unit_test_TIVA
 *
 * Author:: Arundhathi Swami, Vignesh Jayaram
 *
 * Credits: Andy Kobyljanec
 * 
 * Basic source file for testing I2C read and write
 *
 * 
 */



#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
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


// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*user defined libraries included*/
#include "UserI2CLib.h"

/* macro definitions for various tasks*/

/*Ultrasonic Sensor Task*/
#define MAX_TIME 7500

/*ADXLSensor Task*/
/*register set definitions*/
#define DEVADDR 0x53
#define INTENABLE 0x2E



void ADXLI2CInit(void)
{
    /* Using I2C1 I2C on board at the muxed pins of base PORT G
     * Pin 0 - SCL
     * Pin 1 - SDA
     *  */

    /*Enabling the peripheral*/
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    /* Idling till peripheral is ready to be used */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG));

    /*Enabling the use of GPIO pins PG0 and PG1 as I2C1 SCL and I2C1 SDA using muxing*/
    ROM_GPIOPinConfigure(GPIO_PG0_I2C1SCL);
    ROM_GPIOPinConfigure(GPIO_PG1_I2C1SDA);

    /*Setting up these pins to perform I2C functioning (sets them as open drain pins with weak pull up
     * Ref Consult the data sheet to see which functions are allocated per pin.
     */
    ROM_GPIOPinTypeI2C(GPIO_PORTG_BASE, GPIO_PIN_1);
    GPIOPinTypeI2CSCL(GPIO_PORTG_BASE, GPIO_PIN_0);

    /* Disable, Reset and reEnable I2C Module in Master configuration */
    ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C1);
    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    /* Idling till the Peripheral is ready */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    /* Initializing and Configuring the Master I2C1 */
    ROM_I2CMasterInitExpClk(I2C1_BASE, SYSTEM_CLOCK, true);

}


// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    /*initialise accelerometer*/
    ADXLI2CInit();

    UARTprintf("\r\n----------------------I2C UNIT TESTING-------------------\n");

    /*reading DEvice id register and checking its default value*/
    uint32_t identity = readReg(DEVADDR, INTENABLE);
    if(identity == 0xe5)
    {
        UARTprintf("\r\nI2C UNIT TEST FOR READ: SUCCESFULL");
    }

    uint32_t value = 0;
    uint32_t enabledints = 0;
    
    /*Writing to device id register and reading it back, to test write*/
    value = writeReg(DEVADDR, INTENABLE, 0x18);
    enabledints = readReg(DEVADDR, INTENABLE);

    if(enabledints == value)
    {
        UARTprintf("\r\nI2C UNIT TEST FOR WRITE: SUCCESFULL");
    }

    SysCtlDelay(120000); //Delay by 10us

    return 0;
}



/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
