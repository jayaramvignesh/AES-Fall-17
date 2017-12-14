/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserADXL.c
*
*   description: User defined functions tailored to ADXL-GYS 345 accelerometer sensor initialization and reading using
*    Tivaware source code
*
*
*
*****************************************************************************/


#include "UserADXL.h"


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
    ROM_I2CMasterInitExpClk(I2C1_BASE, SYSTEM_CLOCK, true);     //400kbps

}

void ADXLSInit(void)
{
    /*set up i2c  for adxl input*/
        ADXLI2CInit();
        UARTprintf("\n\n\n");
        /*reading device id*/
        uint32_t deviceid = readReg(DEVADDR, DEVID);
        UARTprintf("Device ID: 0x%x\n", deviceid);

        /*data format register*/
        /*reading data format reg*/
        uint32_t df = readReg(DEVADDR, DATAFORMAT);
        UARTprintf("Initial Data Format Reg: 0x%x\n", df);

        /*write 0x01 to the data format register to put it into +/- 4G range*/
       uint32_t value = writeReg(DEVADDR,DATAFORMAT, 0x01);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", DATAFORMAT, 0x01);

        /*reading data format reg*/
        df = readReg(DEVADDR, DATAFORMAT);
        UARTprintf("Data Format Reg: 0x%x\n", df);


        /*int enable register*/
        /*reading int enable reg*/
        uint32_t ie = readReg(DEVADDR, INTENABLE);
        UARTprintf("Initial Int Enable Reg: 0x%x\n", ie);

        /*write 0x00 to disable interrupts while setting thresholds*/
        value = writeReg(DEVADDR, INTENABLE, 0x00);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTENABLE, 0x00);

        /*reading int enable reg*/
        ie = readReg(DEVADDR, INTENABLE);
        UARTprintf("Int Enable Reg: 0x%x\n", ie);

        /*Set a threshold of 1g for inactivity*/
        uint32_t inactthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("InActivity Threshold Value Reg: 0x%x\n", inactthresh);

        value = writeReg(DEVADDR, THRESHINACT, 0x08);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", THRESHINACT, 0x08);

        inactthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("InActivity Threshold Value Reg: 0x%x\n", inactthresh);

        /*Set a threshold of 3g for activity*/
        uint32_t actthresh = readReg(DEVADDR, THRESHACT);
        UARTprintf("Activity Threshold Value Reg: 0x%x\n", actthresh);

        value = writeReg(DEVADDR, THRESHACT, 0x0F);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", THRESHACT, 0x0F);

        actthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("Activity Threshold Value Reg: 0x%x\n", actthresh);

        /*Raise ACT event immediately (defaults to a 1 second delay)*/
        uint32_t tinact = readReg(DEVADDR, TIMEINACT);
        UARTprintf("Timer for Inactivity Value Reg: 0x%x\n", tinact);

        value =  writeReg(DEVADDR, TIMEINACT,0);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", TIMEINACT, 0x00);

        tinact = readReg(DEVADDR, TIMEINACT);
        UARTprintf("Timer for Inactivity Value Reg: 0x%x\n", tinact);

        /*Consider all axes AND compare THRESH_INACT from 0 acceleration (otherwise it would be differential)*/
        uint32_t actinact = readReg(DEVADDR, ACTINACTCTL);
        UARTprintf("Act Inact Control Reg: 0x%x\n", actinact);

        value = writeReg(DEVADDR, ACTINACTCTL, 0x77);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", ACTINACTCTL, 0x77);

        actinact = readReg(DEVADDR, ACTINACTCTL);
        UARTprintf("Act Inact Control Reg: 0x%x\n", actinact);

        /*Set all interrupts for activity to INT 1 (PIN 2) by writing 0 to all bits. If 1 is written, all interrupts are sent to INT2*/
        uint32_t intmapping = readReg(DEVADDR, INTMAP);
        UARTprintf("Interrupt Map: 0x%x\n",intmapping);

        value = writeReg(DEVADDR, INTMAP,0x00);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTMAP, 0x00);

        intmapping = readReg(DEVADDR, INTMAP);
        UARTprintf("Interrupt Map: 0x%x\n",intmapping);


        /* Enable interrupts for activity and inactivity only */
        uint32_t enabledints = readReg(DEVADDR, INTENABLE);
        UARTprintf("Enabled Interrupts: 0x%x\n", enabledints);

        value = writeReg(DEVADDR, INTENABLE, 0x18);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTENABLE, 0x18);

        enabledints = readReg(DEVADDR, INTENABLE);
        UARTprintf("Enabled Interrupts: 0x%x\n", enabledints);

        /*Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register
         * and using linked (serial) activity inactivity detection*/
        value = writeReg(DEVADDR, PWRCTRL, 0x28);  //Measurement mode
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", PWRCTRL, 0x28);

        /*read only register is cleared when read*/
        uint8_t intsrc = readReg(DEVADDR, INTSOURCE); //Clear the interrupts from the INT_SOURCE register.
        UARTprintf("Active Interrupts: 0x%x\n", intsrc);

}
