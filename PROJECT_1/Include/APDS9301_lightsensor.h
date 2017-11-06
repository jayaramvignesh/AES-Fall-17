/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 5 november 2017 
 *
 * File: library file for APDS9301 LightSensor 
 * Description: Contains function declarations and macros to switch on the module, configure its attributes and read temperature from the sensor in human readable values. 
 * * *******************************/
#ifndef _APDS9301_lightsensor_H
#define _APDS9301_lightsensor_H

#include"ERROR_LIB.h"
#include"BBG_I2C2.h"


#define DEVICE_ADDR 0x39

uint8_t WRITE_BUFF[MAX_BUFFER_SIZE]; 
uint8_t DATA_BUFF[MAX_BUFFER_SIZE]; 
uint8_t WR_BUFF[MAX_BUFFER_SIZE]; 
uint8_t RD_BUFF[MAX_BUFFER_SIZE]; 

#define POWER_UP 0x03
#define POWER_DOWN 0x00
#define REG_TH_LOW_LOWER 0x82
#define REG_TH_LOW_HIGHER 0x83
#define REG_TH_HIGH_LOWER 0x84 
#define REG_TH_HIGH_HIGHER 0x85
#define REG_INTERRUPT 0x86
#define REG_TIMING 0x81
#define REG_CONTROL 0x80
#define REG_ID 0xAA
#define REG_SENSOR0_LOWER 0xAC
#define REG_SENSOR0_UPPER 0xAD
#define REG_SENSOR1_LOWER 0xAE
#define REG_SENSOR1_UPPER 0xAF


/*Values for registers*/ 
#define TH_LOW_LOWER 0x0f
#define TH_LOW_HIGHER 0x00
#define TH_HIGH_LOWER 0x00
#define TH_HIGH_HIGHER 0x08
#define PERSISTENCE 0x04 /*if the luminosity crosses threshold for 4 consecutive periods*/ 
#define ENABLE_INTR 0x01/*Interrupts enabled*/
#define DISABLE_INTR 0x00 /*interrupts disabled*/ 
#define GAIN 0x00/*low gain*/
#define INTEGRATION_TIME 0x01/*Integration time per conversion 101ms*/
#define INTERRUPT_VALUE ENABLE_INTR<<4 | PERSISTENCE 
#define TIMING_VALUE GAIN << 4 | INTEGRATION_TIME

/*Threshold values for comparison*/
#define BRIGHT_LIGHT 0x0080
#define LOW_LIGHT 0x000f

/*Function Definitions*/

/*1. Funtion: sets up sensor and opens file to start reading and writing from/to the sensor*/ 
 int InitLightSensor(void);

/*2. Function: Writes to command register before every read and write*/ 
error_t writeCommandReg(int file, uint8_t reg);

/*3. Function: Writes to ID register to get data about sensor*/ 
error_t readIDReg(int file); 

/*4. Function: Configures sensor with default set up values sensor*/ 
error_t configureSensorDefault(int file);

/*5. Function: Configure Integration Time from timing register*/
error_t setIntTime(int file, int time); 

/*6. Function: reads values of register*/ 
error_t readReg(int file, uint8_t reg);

/*7. Function: Sets interrupt control register*/ 
error_t setIntrptControl(int file, int action); 

/*8. Function: PowerUP the sensor*/ 
error_t PowerUP(int file); 

/*9. Function: Shut Down the sensor*/ 
error_t PowerDown(int file); 

/*10.Function: reads lux value*/ 
float readLux(int file); 
#endif 
