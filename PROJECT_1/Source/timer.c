/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
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
/*handler for timer*/
void timer_handler(int signum)
{
  static sig_atomic_t timer_temp_count = 0;
  static sig_atomic_t timer_light_count = 0;
  timer_temp_count++;
  timer_light_count++;
  
  if(timer_temp_count == temp_task_period)
  {
    pthread_cond_broadcast(&temp_task_cond);
    timer_temp_count = 0;
  }

  if(timer_light_count == light_task_period)
  {
    pthread_cond_broadcast(&light_task_cond);
    timer_light_count = 0;
  }
}

/*function to setup timer*/
struct itimerval  setup_timer(uint32_t sec_value, uint32_t usec_value)
{
  struct itimerval timer;
  struct sigaction sa; 

  printf("\n%d SEC VALUE\n",sec_value);
  printf("\n%d USEC VALUE\n",usec_value);
 /*clear out the sigaction instance*/
  memset(&sa,0,sizeof(sa));

  /*set the handler*/
  sa.sa_handler = &timer_handler;
  sigaction(SIGALRM, &sa, NULL);

  /*set a periodic timer for 500 ms*/
  timer.it_value.tv_sec = sec_value;
  timer.it_value.tv_usec = usec_value;

  timer.it_interval.tv_sec = sec_value;
  timer.it_interval.tv_usec = usec_value;
  
  return timer;
}



