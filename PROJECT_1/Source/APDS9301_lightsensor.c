/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 5 november 2017 
 *
 * File: library file for APDS9301 LightSensor 
 * Description: Contains function declarations and macros to switch on the module, configure its attributes and read temperature from the sensor in human readable values. 
 * * *******************************/


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdint.h> 
#include<sys/ioctl.h> 
#include<linux/i2c-dev.h>
#include"ERROR_LIB.h"
#include"BBG_I2C2.h" 
#include"APDS9301_lightsensor.h"
#include<math.h> 

/*1. Funtion: sets up sensor and opens file to start reading and writing from/to the sensor*/ 
int InitLightSensor(void)
{
  int file;
  char filename[20];
  snprintf(filename, 19, "/dev/i2c-%d", I2C_NUM);
  file = InitI2C2Slave(filename, DEVICE_ADDR_LIGHT);
  if (file < 0)
  {
    return ESendFailed();
  }
  printf("init: %d\n", file); 
  error_t ret  = setupI2C2Slave(file, DEVICE_ADDR_LIGHT);
 if (ret < 0  || ret == SLAVE_INIT_FAILED)
  {
    return ESendFailed();
  }

  printf("setup: %d\n", ret); 
  return file; 
}

/*2. Function: Writes to command register before every read and write*/ 
error_t writeCommandReg(int file, uint8_t reg)
{
  WRITE_BUFF[0] = reg; 
  error_t ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], 0, 1);
 if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
 else 
 {
   return ESendSuccess(); 
 }

}

/*3. Function: Writes to ID Register to get data about sensor*/ 
error_t readIDReg(int file)
{
  WRITE_BUFF[0] = REG_ID; 
  error_t ret = writeCommandReg(file, WRITE_BUFF[0]);
  if (ret < 0 || ret == FAILURE)
  {
    return ESendFailed();
  }
 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  uint8_t idvalue = READ_BUFF[0]; 
  uint8_t partno = READ_BUFF[0] >> 4; 
  uint8_t revno =  idvalue & 0x0f;  
  printf("Part Number: %x\n", partno); 
  printf("Revision Number: %x\n", revno); 
  return ESendSuccess(); 
}

/*4. Function: Configures sensor with default set up values  sensor*/ 
error_t configureSensorDefault(int file) 
{
  /*writing to control reg to ppower up*/ 
  error_t ret = writeCommandReg(file, REG_CONTROL); 
   if (ret < 0 || ret == FAILURE)
  {
    return ESendFailed();
  }
 

  WRITE_BUFF[0] = REG_CONTROL; //command reg to write to control register    
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], POWER_UP, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
 

  /* writing to the timing register*/
  WRITE_BUFF[0] = REG_TIMING; //command reg to write to control register   
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TIMING_VALUE, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
 

  /* writing to lower byte of lower threshold*/
  WRITE_BUFF[0] = REG_TH_LOW_LOWER; //command write to lower byte of low th
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TH_LOW_LOWER, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  /* writing to higher byte of lower threshold*/
  WRITE_BUFF[0] = REG_TH_LOW_HIGHER; //command to higher byte of lower th    
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TH_LOW_HIGHER, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  /* writing to lower byte of higher threshold*/
  WRITE_BUFF[0] = REG_TH_HIGH_LOWER; //command write to lower byte high th   
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TH_HIGH_LOWER, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  } 

  /* writing to higher byte of higher threshold*/
  WRITE_BUFF[0] = REG_TH_HIGH_HIGHER; //command write to lower byte high th  
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TH_HIGH_HIGHER, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  
  /* writing to interrupt reg*/
  WRITE_BUFF[0] = REG_INTERRUPT; //command write to interrupt reg    
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0],INTERRUPT_VALUE , 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  } 
  return ESendSuccess(); 
}

/*5. Function: Configure Integration Time from timing register*/
error_t setIntTime(int file, int time) 
{
  if(time>3)
  {
    return ESendFailed(); 
  }
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  WRITE_BUFF[0] = REG_TIMING; 
  error_t ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  uint8_t timingregvalue = READ_BUFF[0];
  timingregvalue &= 0xFC; //zeros out value
  timingregvalue |= (time>>6);

 /* writing to the timing register*/
  WRITE_BUFF[0] = REG_TIMING; //command reg to write to control register   
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], TIMING_VALUE, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
 
  return ESendSuccess(); 
}

/*6. Function: reads values of register*/ 
error_t readReg(int file, uint8_t reg)
{
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  if (reg > 0xAF)
  {
    return ESendFailed(); 
  }
  error_t ret = readI2C2(file, DEVICE_ADDR_LIGHT, reg, READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  else
  {
    printf("Reg %x Value: %x\n", reg, READ_BUFF[0]); 
    return ESendSuccess(); 
  }
}

/*7. Function: Sets interrupt control register*/ 
error_t setIntrptControl(int file, int action)
{
  if(action > 1)
  {
    return ESendFailed(); 
  }
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  WRITE_BUFF[0] = REG_INTERRUPT; 
  error_t ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  
  uint8_t intrregvalue = READ_BUFF[0]; 
  intrregvalue &= 0xCF; //zeros out field
  intrregvalue |= (action<<6); //sets value 
  ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], intrregvalue, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 
}
      
      
      
/*8. Function: PowerUP the sensor*/ 
error_t PowerUP(int file)
{
  WRITE_BUFF[0] = REG_CONTROL; //command reg to write to control register    
  error_t ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], POWER_UP, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 
}

/*9. Function: Shut Down the sensor*/ 
error_t PowerDown(int file) 
{
  WRITE_BUFF[0] = REG_CONTROL; //command reg to write to control register    
  error_t ret = writeI2C2byte(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], POWER_DOWN, 2); 
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 
}

/*10.Function: reads lux value*/ 
float readLux(int file)
{
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  float luxratio; 
  float  sensorlux; 
  uint8_t sensor0lower, sensor0upper, sensor1lower, sensor1upper; 
  uint16_t sensor0, sensor1; 

/*data 0 lower*/ 
  WRITE_BUFF[0] = REG_SENSOR0_LOWER; 
  error_t ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  sensor0lower = READ_BUFF[0]; 
  memset(READ_BUFF, '\0', MAX_BUFFER_SIZE); 

  /*data 0 upper*/ 
 WRITE_BUFF[0] = REG_SENSOR0_UPPER; 
   ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  sensor0upper = READ_BUFF[0]; 
  memset(READ_BUFF, '\0', MAX_BUFFER_SIZE); 

  /*data 1 lower*/ 
 WRITE_BUFF[0] = REG_SENSOR1_LOWER; 
  ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  sensor1lower = READ_BUFF[0]; 
  memset(READ_BUFF, '\0', MAX_BUFFER_SIZE); 

  /*data 1 upper*/ 
 WRITE_BUFF[0] = REG_SENSOR1_UPPER; 
  ret = readI2C2(file, DEVICE_ADDR_LIGHT, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  sensor1upper = READ_BUFF[0]; 
  memset(READ_BUFF, '\0', MAX_BUFFER_SIZE); 

  /*calculating*/ 
  sensor0 = (sensor0upper<<8) | sensor0lower;
  sensor1 = (sensor1upper<<8) | sensor1lower;
 
  luxratio = (float)sensor1/(float)sensor0; 
  if(luxratio>1.30)
  {
    sensorlux = 0; 
    printf("Light Minimal\n"); 
  }
  else if(luxratio > 0 && luxratio <= 0.50)
  {
    sensorlux = (0.0304*sensor0) - (0.062*sensor0*(powf(luxratio,1.4))); 
  }
  else if(luxratio > 0.50 && luxratio <=0.610)
  {
    sensorlux = ((0.0224*sensor0) - (0.031*sensor1));
  }
  else if(luxratio > 0.61 && luxratio <= 0.80)
  {
    sensorlux = ((0.0128*sensor0) - (0.0153*sensor1));
  }
  else if (luxratio > 0.80 && luxratio <= 1.30)
  {
    sensorlux = ((sensor0* 0.00146) - (0.00112*sensor1)); 
  }
  return sensorlux; 

}
