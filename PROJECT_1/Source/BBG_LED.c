/*************************************
 * Authors: Vignesh Jayaram, Arundhathi Swami 
 * Date Started: 30 October 2017 
 * Date Last Edited: 4 november 2017 
 *
 * File: library file for LED User - beagle bone green 
 * Description: Contains function definitions to switch on and switch off 3 user leds on the BBG board
 * ***********************************/

#include<stdio.h> 
#include<stdint.h> 
#include<stdlib.h> 
#include"ERROR_LIB.h"
#include"BBG_LED.h"

error_t LEDOn(uint8_t led)
{
  FILE* LEDHandler = NULL;
  uint8_t* LEDPATH;
  if (led == 1)
  {
    LEDPATH= "/sys/class/leds/beaglebone:green:usr1/brightness";
  }
  else if (led == 2)
  {
    LEDPATH = "/sys/class/leds/beaglebone:green:usr2/brightness";
  }
  else if (led == 3)
  {
    LEDPATH = "/sys/class/leds/beaglebone:green:usr3/brightness";
  }
  else 
  { 
    return ESendLedOpStatusFailure(); 
  }
  if(LEDHandler = fopen(LEDPATH, "r+"))
  {
    fwrite("1", 1, 1, LEDHandler); 
    fclose(LEDHandler); 
    return ESendLedOpStatusSuccess(); 
  }
  else 
  {
    return ESendLedOpStatusFailure(); 

  }
}

error_t LEDOff(uint8_t led)
{
  FILE* LEDHandler = NULL; 
  uint8_t* LEDPATH;
  if (led == 1)
  {
    LEDPATH = "/sys/class/leds/beaglebone:green:usr1/brightness";
  }
  else if (led == 2)
  {
    LEDPATH = "/sys/class/leds/beaglebone:green:usr2/brightness";
  }
  else if (led == 3)
  {
    LEDPATH = "/sys/class/leds/beaglebone:green:usr3/brightness";
  }
  else 
  { 
    return ESendLedOpStatusFailure(); 
  }
  if(LEDHandler = fopen(LEDPATH, "r+"))
  {
    fwrite("0", 1, 1, LEDHandler); 
    fclose(LEDHandler);
    return ESendLedOpStatusSuccess(); 

  }
  else 
  {
    return ESendLedOpStatusFailure(); 

  }

}

