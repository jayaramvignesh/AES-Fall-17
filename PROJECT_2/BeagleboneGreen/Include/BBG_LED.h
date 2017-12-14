/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 4 november 2017 
 *
 * File: library file for LED User - beagle bone green 
 * Description: Contains function declarations and macros to switch on and switch off 3 user leds on the BBG board
 * *******************************/
#ifndef _BBG_LED_H
#define _BBG_LED_H

#include"ERROR_LIB.h"
#include<stdint.h> 

  
error_t LEDOn(uint8_t led); 
error_t LEDOff(uint8_t led); 



#endif
