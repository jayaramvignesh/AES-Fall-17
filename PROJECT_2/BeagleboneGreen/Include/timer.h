/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: timer.h
*
*   function prototypes
*   -timer_handler() function for callback when timer expires
*   - setup_timer() function to setup the timer for periodic firing
*   - heartbeat_setup() function to setup the time for pthread_codn_wait
*
*
*****************************************************************************/


#ifndef _TIMER_H
#define _TIMER_H

#include "main.h"

  struct timespec heartbeat_setup(uint32_t second_value, uint32_t nanosec_value);

#endif
