/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 4 november 2017 
 *
 * File: library file for TMP 102 Temperature Sensor 
 * Description: Contains function declarations and macros to switch on the module, configure its attributes and read temperature from the sensor in human readable values. 
 * * *******************************/
#ifndef _TMP102_temperaturesensor_H
#define _TMP102_temperaturesensor_H

#include"ERROR_LIB.h"
#include"BBG_I2C2.h"


#define DEVICE_ADDR 0x48

uint8_t WRITE_BUFF[MAX_BUFFER_SIZE]; 
uint8_t DATA_BUFF[MAX_BUFFER_SIZE]; 
uint8_t WR_BUFF[MAX_BUFFER_SIZE]; 
uint8_t RD_BUFF[MAX_BUFFER_SIZE]; 

/*Register Addresses*/
#define REG_TEMP 0x00 /*read only register that has temp value by default*/ 
#define REG_CONFIGURATION 0x01  /*configuration register*/
#define REG_TLOW 0x02 /*lower byte of threshold*/
#define REG_THIGH 0x03  /*higher byte of threshold*/


/*TMP102 Attributes*/
#define EXTENDED_MODE 0x00 /*0:12 bit temp values (-55C to +128C)*/
#define CONVERSION_RATE 0x03 /*4Hz Conversion rate*/ 
#define WAKEUP 0x00 /*device maintains a continuous conversion state*/ 
#define SHUTDOWN 0x01 /*device is able to go into low power state between one shot conversions*/
#define THERMOSTAT_MODE 0x00 /*device is in comparator mode*/
#define POLARITY_MODE 0x00 /*Makes AL pin active low*/ 
#define FAULTBITS 0x02 /*generates alert after 4 consecutive faults*/
#define CONVERTER_RESOLUTION 0x02 /*Read only bits, sets temperature to a 12 bit resolution*/ 
#define ONESHOT_CR 0x00 /*saves power between conversions when 1*/ 

/*Alert BIT (ro). Provides information about the comparator status. FOR POL = 0, AL will read as 1 till the temp equals or exceeds Thigh for fault no of times*/ 
 
/*Function Definitions*/

/*1. Funtion: sets up sensor and opens file to start reading and writing from/to the sensor*/ 
int InitTempSensor(void); 

/*2. Function: Writes to pointer register before every read and write*/ 
error_t writePointerReg(int file, uint8_t reg);

/* 3. Function: sets extended mode (12/13 bit repesentation of temp) values*/
error_t setEM(uint8_t mode, int file); 

/* 4. Function: sets the rate for conversion of value*/
error_t setConversionRate(uint8_t rate, int file);

/*5. Function: sets the shutdown mode - with 0 being continuous conversion or wake up*/ 
error_t setSDWakeup(int file);

/*6. Function: sets shutdown mode - 1 being low power state between one shot conversions*/ 
error_t setSDSavePower(int file);


/*7. Function: sets sensor in threshold mode (comparator mode) or interrupt mode*/ 
error_t setTM(uint8_t tm_mode , int file);

/*8. Funtion: sets polairty of alert bit (active low or active high)*/ 
error_t setPOL(uint8_t polarity, int file);

/*9. Function: Sets the number of faults (crossing limits) that must occur before alert is set*/
error_t setFault(uint8_t faultvalue, int file);

/*10. Function: sets lower threshold for temperature in Celcius*/ 
error_t setLowerThresholdRegisterC(int file, float lower_threshold);

/*11. Function: sets higher threshold for temperature in celcius*/
error_t setHigherThresholdRegisterC(int file, float higher_threshold);

/*12. Function: sets lower threshold for temperature in farenheit*/
error_t setLowerThresholdRegisterF(int file, float lower_threshold);

/*13. Function: sets higher threshold for temperature in farenheit*/
error_t setHigherThresholdRegisterF(int file, float higher_threshold);

/*14. FUnction: sets lower_threshold for temperature in kelvin*/
error_t setLowerThresholdRegisterK(int file, float lower_threshold);

/*15. Function: sets higher_threshold for temperature in kelvin*/
error_t setHigherThresholdRegisterK(int file, float higher_threshold);

/*16. Function: reads lower threshold set on the sensor in deg C*/ 
float readLowTempThresholdRegC(int file); 

/*17. Function: reads higher temperature threshold set on the sensor in deg C*/ 
float readHighTempThresholdRegC(int file);

/*18. Function: reads lower temperature threshold set on the sensor in deg F*/ 
float readLowTempThresholdRegF(int file);

/*19. Function: reads higher temperature threshold set on the sensor in deg F*/ 
float readHighTempThresholdRegF(int file);

/*20. Function: reads lower temperature threshold set on the sensor in deg K*/ 
float readLowTempThresholdRegK(int file);

/*21. Function: reads higher temperature threshold set on the sensor in deg F*/ 
float readHighTempThresholdRegK(int file);

/*22. Function: reads values from configuration register*/ 
uint16_t readConfigurationRegister(int file);

/*23. Function: reads temp register and returns converted value in celcius units*/ 
float readTempC(int file);

/*24. Function: reads temp register and returns converted value in farenheit units*/ 
float readTempF(int file);

/*25. Function: reads temp register and returns converted value in kelvin units*/ 
float readTempK(int file);

/*26. Function: sets default values to registers in sensor*/ 
int setupTempSensorDefault(void);


#endif 
