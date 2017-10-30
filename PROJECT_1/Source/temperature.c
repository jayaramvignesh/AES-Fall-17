#include "temperature.h"

/*function for thread1*/
void *temperature_function()
{
  while(1)
  {

    pthread_cond_broadcast(&main_thread1_cond);
    time_t a = time(NULL);
    task1.current_time = ctime(&a);
    task1.logged_level = SENSOR_DATA;
    task1.task_ID = temperature_task;
    int data = rand()%100;
    sprintf(task1.message,"%d",data);
    task1.message_length = strlen(task1.message);
  
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread1_mutex);
    
    pthread_cond_wait(&thread1_cond,&thread1_mutex);  
    if(exit_flag == 1)
    {
      break;
    }

    printf("\nInside thread 1\n");  
    
    if(task1.logged_level = SENSOR_DATA)
    {
      /*send the message to the queue and check for success*/
      if(mq_send(mqdes2,(const char *)&task1, sizeof(task1),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
    }

    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&task1, sizeof(task1),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }

   /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread1_mutex);
    printf("\nmessage sent\n");  
  }

  pthread_mutex_unlock(&thread1_mutex);
  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 1\n");
  pthread_exit(NULL);
}



