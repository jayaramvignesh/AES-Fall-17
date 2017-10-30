#include "timer.h"

/*handler for timer*/
void timer_handler(int signum)
{
  main_count++;
  count++;
  printf("\ntimer expired %d times\n",count);
  if(count == 48)
  {
    exit(1);
  }
  if(exit_flag == 1)
  {
    pthread_cond_broadcast(&thread1_cond);
    pthread_cond_broadcast(&thread2_cond);
    pthread_cond_broadcast(&thread3_cond);
    pthread_cond_broadcast(&thread4_cond);
  }

  if(count == 1)
  {
    pthread_cond_broadcast(&thread1_cond);
  }
  else if(count == 2)
  {
    pthread_cond_broadcast(&thread2_cond);
  }
  else if(count == 3)
  {
    pthread_cond_broadcast(&thread3_cond);
    pthread_cond_broadcast(&thread4_cond);
    count  = 0;
  }
}


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
