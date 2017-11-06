/***********************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 27 October 2017 
 * Date Last Edited: 4 November 2017 
 *
 * File: library file for i2c2 - beagle bone green 
 * Description: contains function declarations and macros required to read and write from the i2c serial communication bus i2c0 on the beagle bone green development board
 * ************************************/
#include<stdio.h> 
#include<stdint.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include"ERROR_LIB.h"
#include"BBG_I2C2.h"
#include<string.h> 
#include<fcntl.h>
#include<sys/ioctl.h> 
#include<linux/i2c-dev.h>


int  InitI2C2Slave(uint8_t filename[], uint8_t DevAddr)
{
  int file = open(filename, O_RDWR); 
  if(file<0){
    printf("Error: Cant open file\n");
    return ESendSlaveInitFailure();  
  }
  
  if(ioctl(file, I2C_SLAVE, DevAddr) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return  ESendSlaveInitFailure(); 
  }
  return file;

}

error_t setupI2C2Slave(int file, uint8_t DevAddr)

{
  if (ioctl(file, I2C_SLAVE, DevAddr) < 0)
  {
    printf("Error: Slave Address Resolution\n"); 
    return ESendSlaveInitFailure(); 
  }
  return ESendSlaveInitSuccess(); 
  
}


error_t writeI2C2byte(int file, uint8_t DevAddr, uint8_t Register, uint8_t Data, uint8_t size)
{
   setupI2C2Slave(file, DevAddr); 
   WR_BUFF[0] = Register; 
   WR_BUFF[1] = Data; 
   if(write(file,WR_BUFF, size) == size)
   {
     memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
     memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
     return ESendI2CWriteSuccess();  
   }
   else 
   {
    memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
    memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
    return ESendI2CWriteFailure(); 
   }
}

error_t writeI2C2bytes(int file, uint8_t DevAddr, uint8_t Register, uint16_t Data, uint8_t size)
{
   setupI2C2Slave(file, DevAddr); 
   WR_BUFF[0] = Register; 
   WR_BUFF[1] = (uint8_t)(Data & 0x00ff);//lsb
   WR_BUFF[2] = (uint8_t)((Data & 0xff00)>>8); //msb

   if(write(file,WR_BUFF, size) == size)
   {
     memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
     memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
     return ESendI2CWriteSuccess();  
   }
   else 
   {
    memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
    memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
    return ESendI2CWriteFailure(); 
   }
}

error_t readI2C2(int file, uint8_t DevAddr, uint8_t Register, uint8_t* readbytes)
{
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  WR_BUFF[0] = Register;
  setupI2C2Slave(file, DevAddr); 
  error_t ret; 
  ret = writeI2C2byte(file, DevAddr, WR_BUFF[0],0,1); 
  setupI2C2Slave(file, DevAddr); 
  if(read(file, RD_BUFF, 2) != 2)
  {
    return ESendI2CReadFailure();  
  }
  *readbytes = RD_BUFF[0];
  *(readbytes+1) = RD_BUFF[1]; 
  *(readbytes+2) = RD_BUFF[2]; 
  *(readbytes+3) = RD_BUFF[3]; 
  return ESendI2CReadSuccess(); 
}


