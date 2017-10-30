#include "decision.h"

/*function for thread 4*/
void *decision_function()
{
  while(1)
  {
   
    pthread_cond_broadcast(&main_thread4_cond);
    char buffer[MSG_SIZE];
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread4_mutex);
    pthread_cond_wait(&thread4_cond,&thread4_mutex); 
 
    if(exit_flag == 1)
    {
        break;
    }


    printf("\nInside thread 4\n");  
    /*get the attributes for the queue*/
    mq_getattr(mqdes2,&attr);
    
    /*check if there is any messages in the queue*/
    if(attr.mq_curmsgs != 0)
    {
      static int Msg_no;
      char logger_level_string[200];
      char task_name[200];
      m_log receiver;
      while(attr.mq_curmsgs != 0)
      {
        Msg_no++;
        printf("\nMEESSSSSSSAGEEEEEEEEE%d is :\n",Msg_no);
      
        int n = mq_receive(mqdes2,(char*)&receiver,MSG_SIZE,NULL);
      
        /*check for which task*/
        if(receiver.task_ID == 1)
        {
          printf("\nMAIN TASK LOGGER. NO DECISION TO BE MADE\n");
        }
        else if(receiver.task_ID == 2)
        {
          if(receiver.logged_level == 1)
          {
            int value = atoi(receiver.message);
            if(value < 75 && value > 50)
            {
              printf("\nALERT!!!! ALERT!!!! ALERT!!!!! TEMPERATURE LIMIT EXCEEEDED\n");
            }
          }
        }
        else if(receiver.task_ID == 3)
        {
          if(receiver.logged_level == 1)
          {
            int value = atoi(receiver.message);
            if(value < 50 && value > 25)
            {
              printf("\nALERT!!!! ALERT!!!! ALERT!!!!! LIGHTTT LIMIT EXCEEEDED\n");
            }
          }
        }
        else if(receiver.task_ID == 4)
        {
          printf("\nTASK LOGGER. NO DECISION TO BE MADE\n");
        }

        /*check logger level*/
        if(receiver.logged_level == 1)
        {
          strcpy(logger_level_string,"SENSOR_DATA");
        }
        else if(receiver.logged_level == 2)
        {
          strcpy(logger_level_string,"ERROR");
        }
        else if(receiver.logged_level == 3)
        {
          strcpy(logger_level_string,"INFO");
        }
  
        mq_getattr(mqdes2,&attr);
      }

    }
    
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread4_mutex);
 
    count = 0;

  }
  
  pthread_mutex_unlock(&thread4_mutex);
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 4\n");
  pthread_exit(NULL);

}


