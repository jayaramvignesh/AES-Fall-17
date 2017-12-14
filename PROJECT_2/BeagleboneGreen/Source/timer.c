/****************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: timer.c
*
*   description:
*
*   -source file for timer
*
*   -set up timer for heartbeat (cond timed wait)
*
*   -set up timer for periodic firing
*
*   -set up handler for timer callback when timer fires
*       
*****************************************************************************/

#include "timer.h"

/*Timer setup for Heartbeat*/
struct timespec heartbeat_setup(uint32_t second_value, uint32_t nanosec_value)
{
  struct timespec ts;
  struct timeval tp;

  gettimeofday(&tp,NULL);
  ts.tv_sec = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += second_value;
  ts.tv_nsec += nanosec_value;
  
  return ts;

}


