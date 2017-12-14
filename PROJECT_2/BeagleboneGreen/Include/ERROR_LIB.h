/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 4 November 2017 
 * Date Last Edited: 4 November 2017 
 *
 * File: Library File for Errors 
 * Description: Contains principle enum enumerating all errors specific to utilities for project 1. i.e. I2C, LED and Sensors for Light and Temperature, as well as declarations for functions used to send errors
 * ***********************************/
#ifndef _ERROR_LIB_H
#define _ERROR_LIB_H

typedef enum error_list{
  SUCCESSFUL, 
  FAILURE,
  SLAVE_INIT_FAILED, 
  SLAVE_INIT_SUCCEEDED,
  WRITE_FAILED,
  READ_FAILED,
  WRITE_SUCCEEDED, 
  READ_SUCCEEDED, 
  LED_OP_SUCCESSFUL, 
  LED_OP_FAILED, 
}error_t; 


/*Function to send status: Successful*/ 
error_t ESendSuccess(void); 

/*Function to send error: Failed*/
error_t ESendFailed(void); 

/*Function to send slave initialization status: Successful*/ 
error_t ESendSlaveInitSuccess(void); 

/*Function to send slave initialization status: Failure*/ 
error_t ESendSlaveInitFailure(void); 

/*Function to send I2C write status: Success*/ 
error_t ESendI2CWriteSuccess(void); 

/*Function to send I2C write status: Failure*/ 
error_t  ESendI2CWriteFailure(void); 

/*Function to send I2C Read status: Success*/ 
error_t ESendI2CReadSuccess(void); 

/*Function to send I2C Read status: Failure*/ 
error_t  ESendI2CReadFailure(void); 

/*Function to send Led Operation Status Success*/
error_t ESendLedOpStatusSuccess(void); 

/*Function to send Led Operation Status Failure*/ 
error_t ESendLedOpStatusFailure(void); 

#endif
