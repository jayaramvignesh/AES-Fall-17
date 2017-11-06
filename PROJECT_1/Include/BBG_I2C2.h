/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 27 October 2017 
 * Date Last Edited: 4 November 2017 
 *
 * File: library file for i2c2 - beagle bone green 
 * Description: contains function declarations and macros required to read and write from the i2c serial communication bus i2c2 on the beagle bone green development board
 * ************************************/
#ifndef _BBG_I2C2_H
#define _BBG_I2C2_H


#define I2C_NUM 2 /*I2C Instance Number*/
#define MAX_BUFFER_SIZE 4 

extern uint8_t WR_BUFF[MAX_BUFFER_SIZE];extern uint8_t RD_BUFF[MAX_BUFFER_SIZE]; 

/*Function: reading slave register*/
error_t readI2C2(int file, uint8_t DevAddr, uint8_t Register, uint8_t* readbytes);
/*Function: to set up the slave*/
int InitI2C2Slave(uint8_t filename[], uint8_t DevAddr);
/*Function: Open connection to I2C*/ 
error_t setupI2C2Slave(int file, uint8_t DevAddr);
/*Function: Writing to slave Register*/
error_t writeI2C2byte(int file, uint8_t DevAddr, uint8_t Register, uint8_t data, uint8_t size);
error_t writeI2C2bytes(int file, uint8_t DevAddr, uint8_t Register, uint16_t data, uint8_t size);
 

#endif
