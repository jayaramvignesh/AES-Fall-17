#include "light.h"

/*function for thread2*/
void *light_function()
{
  while(1)
  {

    pthread_cond_broadcast(&main_thread2_cond);
    time_t a = time(NULL);
    task2.current_time = ctime(&a);
    task2.logged_level = SENSOR_DATA;
    task2.task_ID = light_task;
    int data = rand()%100;
    sprintf(task2.message,"%d",data);
    task2.message_length = strlen(task2.message);
    
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread2_mutex);
    pthread_cond_wait(&thread2_cond,&thread2_mutex); 
    if(exit_flag == 1)
    {
      break;
    }
    printf("\nInside thread 2\n");  

    if(task2.logged_level = SENSOR_DATA)
    {
      /*send the message to the queue and check for success*/
      if(mq_send(mqdes2,(const char *)&task2, sizeof(task2),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
 
    }

   /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&task2, sizeof(task2),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
 
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread2_mutex);
  
    printf("\nmessage sent\n");  
  }
  
  pthread_mutex_unlock(&thread2_mutex);
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDD 2\n");
  pthread_exit(NULL);
}


