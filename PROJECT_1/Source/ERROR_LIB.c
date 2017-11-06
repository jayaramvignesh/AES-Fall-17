/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 4 November 2017 
 * Date Last Edited: 4 November 2017 
 *
 * File: Source File for Errors 
 * Description: Contains defintions for functions used to send errors specific to BBG utilities I2C, LED, temperature and light sensors. 
 * ***********************************/


#include<stdio.h> 
#include<stdint.h> 
#include<stdlib.h> 
#include"ERROR_LIB.h"

/****************
 * Function: send generic operation status: Successful
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendSuccess(void)
{
  printf("Operation Status: Successful\n"); 
  return SUCCESSFUL; 
}

/****************
 *Function: send generic operation status: failure
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendFailed(void)
{
  printf("Operation Status: Failed\n"); 
  return FAILURE; 
}

/****************
 * Function to send slave initialization status: Successful
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendSlaveInitSuccess(void) 
{
 // printf("Slave Initialization Status: Successful\n"); 
  return SLAVE_INIT_SUCCEEDED; 
}

/****************
 * Function to send slave initialization status: Failure
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendSlaveInitFailure(void) 
{
  //printf("Slave Initialization Status: Successful\n"); 
  return SLAVE_INIT_FAILED; 
}


/****************
 *Function: Sends I2C write status: Success
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendI2CWriteSuccess(void) 
{
  //printf("I2C Write Status : Successful\n"); 
  return WRITE_SUCCEEDED; 
}

/****************
 *Function: Sends I2C write status: Failure
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendI2CWriteFailure(void) 
{
 // printf("I2C Write Status: Failed\n"); 
  return WRITE_FAILED; 
}

/****************
 * Function: Sends I2C Read status: Success 
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendI2CReadSuccess(void) 
{
  //printf("I2C Read Status: Successful\n"); 
  return READ_SUCCEEDED; 
}


/****************
 * Function: Sends I2C Read status: Failure
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendI2CReadFailure(void) 
{
  //printf("I2C Read Status : Failed\n"); 
  return READ_FAILED; 
}


/****************
 * Function: Send Led Operation Status: Success
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendLedOpStatusSuccess(void)
{
  //printf("Led Operation Status : Successful\n"); 
  return LED_OP_SUCCESSFUL; 
}

/****************
 * Function: Send Led Operation Status: Failure
 * Return Type: error_t (enum for errors)
 * Formal Arguments: void 
 * *************/ 
error_t ESendLedOpStatusFailure(void)
{
  //printf("Led Operation Status : Failure\n"); 
  return LED_OP_FAILED; 
}

