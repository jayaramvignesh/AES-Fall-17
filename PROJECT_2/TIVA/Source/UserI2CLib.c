/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: UserI2CLib.c
*
*   description: User defined functions tailored to current project using I2C driver lib from Tivaware source code
*
*
*****************************************************************************/

#include"UserI2CLib.h"

uint32_t base = I2C1_BASE;

void getI2CNo(uint32_t no)
{
    if (no == 0)
    {
        base = I2C0_BASE;
    }
    else if (no == 1)
    {
        base = I2C1_BASE;
    }
    else if (no == 2)
    {
        base = I2C2_BASE;
    }
}
uint32_t readReg(uint32_t devaddr, uint32_t reg)
{
    ROM_I2CMasterSlaveAddrSet(base, devaddr, false);   //write to accelerometer
    ROM_I2CMasterDataPut(base,reg);
    ROM_I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_START);
    while(ROM_I2CMasterBusy(base));

    SysCtlDelay(100); //Delay by 1us

    ROM_I2CMasterSlaveAddrSet(base, devaddr, true);    //read from status sensor
    ROM_I2CMasterControl(base, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(ROM_I2CMasterBusy(base));
    uint32_t value = ROM_I2CMasterDataGet(base);
    return value;

}


uint32_t writeReg(uint32_t devaddr, uint32_t reg, uint32_t value)
{
    ROM_I2CMasterSlaveAddrSet(base, devaddr, false);   //write to adxl at address 0x53
    ROM_I2CMasterDataPut(base, reg); // reg
    ROM_I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_START);
    while(ROM_I2CMasterBusy(base));

    SysCtlDelay(100); //Delay by 1us

    ROM_I2CMasterDataPut(base, value); //disabling low power functioning mode
    ROM_I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(ROM_I2CMasterBusy(base));

    return 0;
}
