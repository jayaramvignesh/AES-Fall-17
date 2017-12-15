/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserUSS.c
*
*   description: User defined functions tailored to HCSR04 ultrasonic distance sensor initialization and reading using
*    Tivaware source code
*
*
*
*****************************************************************************/

#include "UserUSS.h"

void USSGPIOInit(void)
{
    //Port C and E for GPIO r/w
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);


    //enable the gpio peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);//gpio c set
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);//gpio c set


    //wait for gpio modules to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
    {
    }


    //sets pin pc6 as digital output to trig pin of sensor
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);

    //sets pin pl3 as analog input to echo pin of sensor
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_3);
}
