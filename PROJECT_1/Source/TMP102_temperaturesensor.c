/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 4 november 2017 
 *
 * File: source file for TMP102 Temperature Sensor 
 * Description: Contains function definitions to switch on the module, configure its attributes and read temperature from the sensor in human readable values
 * ****************************/

#include<stdio.h>
#include<stdint.h> 
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h> 
#include<linux/i2c-dev.h> 
#include"ERROR_LIB.h"
#include"BBG_I2C2.h"
#include"TMP102_temperaturesensor.h"

/*1. Funtion: sets up sensor and opens file to start reading and writing from/to the sensor*/ 
int InitTempSensor(void)
{
  int file;
  char filename[20];
  snprintf(filename, 19, "/dev/i2c-%d", I2C_NUM);
  file = InitI2C2Slave(filename, DEVICE_ADDR);
  if (file < 0)
  {
    return ESendFailed();
  }
  printf("init: %d\n", file); 
  error_t ret  = setupI2C2Slave(file, DEVICE_ADDR);
 if (ret < 0  || ret == SLAVE_INIT_FAILED)
  {
    return ESendFailed();
  }

  printf("setup: %d\n", ret); 
  return file; 
}

/*2. Function: Writes to pointer register before every read and write*/ 
error_t writePointerReg(int file, uint8_t reg)
{
  WRITE_BUFF[0] = reg; 
  error_t ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], 0, 1);
 if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
 else 
 {
   return ESendSuccess(); 
 }

}


/*3. Function: sets extended mode (12/13 bit repesentation of temp) values*/
error_t setEM(uint8_t mode, int file)
{

  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0];
  DATA_BUFF[2] = READ_BUFF[1];
  DATA_BUFF[2] &= 0xEF;
  DATA_BUFF[2] |= mode<<4;
  uint16_t writedata = ((uint16_t)(DATA_BUFF[1] << 8))| DATA_BUFF[2]; 
  ret = writeI2C2bytes(file, DEVICE_ADDR, WRITE_BUFF[0], writedata, 3);
  if (ret < 0 || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  return ESendSuccess(); 
}

/*4. Function: sets the rate for conversion of value*/
error_t setConversionRate(uint8_t rate, int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0];
  DATA_BUFF[2] = READ_BUFF[1];
  DATA_BUFF[2] &= 0x3F; /*clearing out cr bits */
  DATA_BUFF[2] |= rate << 6;
  uint16_t writedata = ((uint16_t)(DATA_BUFF[1] << 8)) | DATA_BUFF[2]; 
  ret = writeI2C2bytes(file, DEVICE_ADDR, WRITE_BUFF[0], writedata, 3);
 if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
 return ESendSuccess(); 
}

/*5. Function: sets the shutdown mode - with 0 being continuous conversion or wake up*/ 
error_t setSDWakeup(int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);  
 if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0]; 
  DATA_BUFF[1] &= 0xFE; //clears out the sd bit  
  DATA_BUFF[1] |= WAKEUP; 
  ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], DATA_BUFF[1], 2); 
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  return ESendSuccess(); 
}


/*6. Function: sets shutdown mode - 1 being low power state between one shot conversions*/ 
error_t setSDSavePower(int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);  
 if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0]; 
  DATA_BUFF[1] &= 0xFE; //clears out the sd bit  
  DATA_BUFF[1] |= SHUTDOWN; 
  ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], DATA_BUFF[1], 2);
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  return ESendSuccess(); 
}


/*7. Function: sets sensor in threshold mode (comparator mode) or interrupt mode*/ 
error_t setTM(uint8_t tm_mode , int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR,  WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0]; 
  DATA_BUFF[1] &= 0xFD; 
  DATA_BUFF[1] |= tm_mode<<1; 
  ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], DATA_BUFF[1], 2); 
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE);  
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE);
  return ESendSuccess(); 
}

/*8. Function: sets polairty of alert bit (active low or active high)*/ 
error_t setPOL(uint8_t polarity, int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);  
  if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0]; 
  DATA_BUFF[1] &= 0xFB;
  DATA_BUFF[1] |= polarity<<2; 
  ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], DATA_BUFF[1], 2);
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  return ESendSuccess(); 
}

/*9. Function: Sets the number of faults (crossing limits) that must occur before alert is set*/
error_t setFault(uint8_t faultvalue, int file)
{
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  DATA_BUFF[1] = READ_BUFF[0]; 
  DATA_BUFF[1] &= 0xE7;
  DATA_BUFF[1] |= faultvalue<<3;
  ret = writeI2C2byte(file, DEVICE_ADDR, WRITE_BUFF[0], DATA_BUFF[1], 2); 
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  return ESendSuccess(); 
}

/*10. Function: sets lower threshold for temperature in Celcius*/ 
error_t setLowerThresholdRegisterC(int file, float lower_threshold)
{
  
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  /*makes sure temp values stay within range*/
  if (lower_threshold > 150.0f)
  {
    lower_threshold = 150.0f; 
  }
  if(lower_threshold < -55.0f)
  {
    lower_threshold = -55.0f;
  }
  /*read current lower threshold register*/
  WRITE_BUFF[0] = REG_CONFIGURATION; 

  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);  
 if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  /*Reading the extended bit value from the config register*/
  uint8_t extendedmode = ((READ_BUFF[1] & 0x10) >> 4);
 
  /*Change register to lower threshold register*/
  WRITE_BUFF[0] = REG_TLOW;

  /*if extended bit is 0, temp data will be 12 bits else 13 bits, so according to that we will adjust register values for lower threshold registers*/ 
  lower_threshold = lower_threshold/0.0625; 
  if (extendedmode) /*13 bit data*/
  {
    DATA_BUFF[1] = (int)(lower_threshold) >> 5;
    DATA_BUFF[2] = (int)(lower_threshold) << 3;
  }
  else 
  { 
    DATA_BUFF[1] = (int)(lower_threshold) >> 4;
    DATA_BUFF[2] = (int)(lower_threshold) << 4;
  } 
  uint16_t writedata = ((uint16_t)(DATA_BUFF[1]<<8) | DATA_BUFF[2]);
   ret = writeI2C2bytes(file, DEVICE_ADDR, WRITE_BUFF[0], writedata, 3); 
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }

  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  return ESendSuccess(); 
}


/*11. Function: sets higher threshold for temperature in celcius*/
error_t setHigherThresholdRegisterC(int file, float higher_threshold)
{
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  /*makes sure temp values stay within range*/
  if (higher_threshold > 150.0f)
  {
    higher_threshold = 150.0f; 
  }
  if(higher_threshold < -55.0f)
  {
    higher_threshold = -55.0f;
  }
  /*read current lower threshold register*/
  WRITE_BUFF[0] = REG_CONFIGURATION;

  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);  
  if (ret < 0  || ret == READ_FAILED)
  {
    return ESendFailed();
  }
  /*Reading the extended bit value from the config register*/
  uint8_t extendedmode = (READ_BUFF[1] &0x10)>>4;
  WRITE_BUFF[0] = REG_THIGH;

  /*if extended bit is 0, temp data will be 12 bits else 13 bits, so according to that we will adjust register values for lower threshold registers*/ 
  higher_threshold = higher_threshold/0.0625; 
  if (extendedmode) /*13 bit data*/
  {
    DATA_BUFF[1] = (int)(higher_threshold) >> 5;
    DATA_BUFF[2] = (int)(higher_threshold) << 3;
  }
  else 
  { 
    DATA_BUFF[1] = (int)(higher_threshold) >> 4;
    DATA_BUFF[2] = (int)(higher_threshold) << 4;
  } 

  uint16_t writedata = ((uint16_t)(DATA_BUFF[1]<<8)) | DATA_BUFF[2];
  ret = writeI2C2bytes(file, DEVICE_ADDR, WRITE_BUFF[0], writedata, 3); 
  if (ret < 0  || ret == WRITE_FAILED)
  {
    return ESendFailed();
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(DATA_BUFF, '\0', MAX_BUFFER_SIZE); 
  return ESendSuccess(); 
}

/*12. Function: sets lower threshold for temperature in farenheit*/
error_t setLowerThresholdRegisterF(int file, float lower_threshold)
{
  lower_threshold = (((lower_threshold -32) *5)/9); 
  error_t ret = setLowerThresholdRegisterC(file, lower_threshold); 
  if (ret == FAILURE)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 

}

/*13. Function: sets higher threshold for temperature in farenheit*/
error_t setHigherThresholdRegisterF(int file, float higher_threshold)
{
  higher_threshold = (((higher_threshold -32) *5)/9); 
 error_t ret =  setHigherThresholdRegisterC(file, higher_threshold);
 if (ret == FAILURE)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 

}

/*14. Function: sets lower_threshold for temperature in kelvin*/
error_t setLowerThresholdRegisterK(int file, float lower_threshold)
{
  lower_threshold = (lower_threshold-273.15) ; 
  error_t ret = setLowerThresholdRegisterC(file, lower_threshold);
 if (ret == FAILURE)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 

}


/*15. Function: sets higher_threshold for temperature in kelvin*/
error_t setHigherThresholdRegisterK(int file, float higher_threshold)
{
  higher_threshold = (higher_threshold-273.15); 
 error_t ret =  setHigherThresholdRegisterC(file, higher_threshold);
 if (ret == FAILURE)
  {
    return ESendFailed();
  }
  return ESendSuccess(); 

}

/*16. Function: reads lower threshold set on the sensor in deg C*/ 
float readLowTempThresholdRegC(int file)
{
  float temperature; 
  int extendedmode;
  int tempfromsensor; 
/*find how to read data coming in from the sensor - 12 or 13 bits*/ 
  WRITE_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  extendedmode = (READ_BUFF[1]&0x10) >>4; 
  WRITE_BUFF[0] = REG_TLOW;  
  ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF); 
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  if(extendedmode) //13 bit temperature
  {
    tempfromsensor = ((READ_BUFF[0]<<5) | (READ_BUFF[1] >> 3)); 
    if(tempfromsensor > 0xFFF) //if temp is negative
    {
      tempfromsensor |= 0xE000; //convert to 16 bit 2's complement
    }
  }
  else //12 bit temperature  
  {
    tempfromsensor = ((READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4)); 
    if (tempfromsensor > 0x7FF) //if temp is negative 
    {
      tempfromsensor |= 0xF000; 
    }
  }
  temperature = tempfromsensor * 0.0625;
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE);
  return temperature; 
  
}

/*17. Function: reads higher temperature threshold set on the sensor in deg C*/ 
float readHighTempThresholdRegC(int file)
{
  float temperature; 
  int extendedmode; 
  int tempfromsensor; 
  WRITE_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  extendedmode = (READ_BUFF[1] & 0x10)>> 4;
  WRITE_BUFF[0] = REG_THIGH; 
  ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);
  if (ret < 0 || ret == READ_FAILED)
  {
    return ESendFailed();
  }

  if(extendedmode)
  {
    tempfromsensor = (READ_BUFF[0] << 5) | (READ_BUFF[1] >>3); 
    if(tempfromsensor > 0xFFF)
    {
      tempfromsensor |= 0xE000; 
    }
  }
  else
  {
    tempfromsensor = (READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4); 
    if(tempfromsensor > 0x7FF)
    {
      tempfromsensor |= 0xF000; 
    }
  }
  temperature = tempfromsensor * 0.0625; 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE); 
  return temperature; 
}

/*18. Function: reads lower temperature threshold set on the sensor in deg F*/ 
float readLowTempThresholdRegF(int file)
{
  float rawtemp = readLowTempThresholdRegC(file);
  if (rawtemp == FAILURE)
  {
    return ESendFailed();
  }

  return (rawtemp*(9.0/5.0))+32.0; 	
}

/*19. Function: reads higher temperature threshold set on the sensor in deg F*/ 
float readHighTempThresholdRegF(int file)
{
  float rawtemp = readHighTempThresholdRegC(file);
  if (rawtemp == FAILURE)
  {
    return ESendFailed();
  }

  return (rawtemp*(9.0/5.0))+32.0; 
}

/*20. Function: reads lower temperature threshold set on the sensor in deg K*/ 
float readLowTempThresholdRegK(int file)
{
  float rawtemp = readLowTempThresholdRegC(file); 
  if (rawtemp == FAILURE)
  {
    return ESendFailed();
  }

  return (rawtemp + 273.15); 
}

/*21. Function: reads higher temperature threshold set on the sensor in deg F*/ 
float readHighTempThresholdRegK(int file)
{
  float rawtemp = readHighTempThresholdRegC(file); 
  if (rawtemp == FAILURE)
  {
    return ESendFailed();
  }

  return (rawtemp + 273.15); 
}

/*22. Function: reads values from configuration register*/ 
uint16_t readConfigurationRegister(int file)
{
  uint16_t configvalue; 
  WRITE_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF);
  if (ret == READ_FAILED )
  {
    return ESendFailed();
  }

  configvalue = (uint16_t)READ_BUFF[0]<<8 | READ_BUFF[1];
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE); 
  return configvalue; 
 }

/*23. Function: reads temp register and returns converted value in celcius units*/ 
float readTempC(int file)
{
  float temperature; 
  int rawtemp; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  WRITE_BUFF[0] =REG_TEMP;
  error_t ret = readI2C2(file, DEVICE_ADDR, WRITE_BUFF[0], READ_BUFF); 
   if (ret == READ_FAILED)
  {
    return ESendFailed();
  }

  if(READ_BUFF[1] & 0x01) //13 bit mode
  {
    rawtemp = (READ_BUFF[0]<<5) | (READ_BUFF[1] >>3); 
    //adjusting for  positive/negative  data
    if(rawtemp > 0x1000)
    {
      rawtemp &= 0x1FFF;
      temperature = (-1) * rawtemp *0.0625;

    }
    else
    {  
      temperature = rawtemp *0.0625;
    }
  }
  else //12 bit mode
  {
    rawtemp = (READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4); 
    if(rawtemp > 0x800)
    {
     rawtemp &= 0x0FFF; 
     temperature = (-1) * rawtemp *0.0625;

    }
    else
    {  
      temperature = rawtemp *0.0625;
    }

  }
  
    memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WRITE_BUFF, '\0', MAX_BUFFER_SIZE); 
  return temperature; 
}

/*24. Function: reads temp register and returns converted value in farenheit units*/ 
float readTempF(int file)
{
  float temp = readTempC(file);
  if ( temp == FAILURE)
  {
    return ESendFailed();
  }

  temp = temp*9.0/5.0+32.0; 
  return temp; 
}

/*25. Function: reads temp register and returns converted value in kelvin units*/ 
float readTempK(int file)
{
  float temp = readTempC(file);
  if ( temp == FAILURE)
  {
    return ESendFailed();
  }
  temp = temp + 273.15;
  return temp; 
}

/*26. Function: sets default values to registers in sensor*/ 
int setupTempSensorDefault(void)
{
 int file = InitTempSensor(); 
 error_t ret = setEM(EXTENDED_MODE, file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setConversionRate(CONVERSION_RATE,file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setSDWakeup(file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setTM(THERMOSTAT_MODE, file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setPOL(POLARITY_MODE, file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setFault(FAULTBITS, file); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setLowerThresholdRegisterC(file, 30.0); 
 if (ret == FAILURE)
   return ESendFailed();
 ret = setHigherThresholdRegisterC(file, 85.0); 
 if (ret == FAILURE)
   return ESendFailed();
 return file; 
}

