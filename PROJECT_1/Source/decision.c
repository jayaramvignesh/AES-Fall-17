/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: decision.c
*
*   description:
*
*   -source file for decision thread
*
*   -this thread sends a signal to release the main thread cond4
*   and to indicate it is alive. This is done at start of while 1
*
*   -exit flag is checked for pthread_exit
*
*   -messages from queues (if there) are read
*
*   -these messages are checked for their source task using log id
*
*   -the data is extracted and checked for condition.
*
*   -if data exceeds condition, appropriate decision is taken
* 
*      
**************************************************************************/


#include "decision.h"

/*function for decsion thread */
void *decision_function()
{
  while(1)
  {
 
    /*send condition signal to main indicating alive*/
    pthread_cond_broadcast(&main_thread4_cond); 

    /*check for graceful exit*/
    if(exit_flag == 1)
    {
        break;
    }

    char buffer[MSG_SIZE];
    
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&decision_queue_mutex);
    
    printf("\nInside thread 4\n");  
    
    /*get the attributes for the queue*/
    mq_getattr(decision_mqdes1,&attr);
    
    /*check if there is any messages in the queue*/
    if(attr.mq_curmsgs != 0)
    {
      static int Msg_no;
      char logger_level_string[200];
      char task_name[200];
      m_log receiver;
     
      /*read messages until queue is empty*/
      while(attr.mq_curmsgs != 0)
      {
        Msg_no++;
      
        /*get the message from queue*/
        int n = mq_receive(decision_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
      
        /*check for which task*/
        if(receiver.task_ID == 1)
        {
          printf("\nMAIN TASK LOGGER. NO DECISION TO BE MADE\n");
        }
        else if(receiver.task_ID == 2)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            /*convert the data from string to INT*/
            int value = atoi(receiver.message);
            
            /*check for condition*/
            if(value < 15 && value > 10)
            {

              time_t a = time(NULL);
              maintask.current_time = ctime(&a);
              maintask.task_ID = decision_task;
              maintask.logged_level = ALERT;
              strcpy(maintask.message_string,"ALERT!!! ALERT!!! ALERT!!! TEMPERATURE IN CELSIUS HAS EXCEEDED LIMIT");
              sprintf(maintask.message,"%d",value);
              maintask.message_length = strlen(maintask.message);  
              printf("\nALERT!!!! ALERT!!!! ALERT!!!!! TEMPERATURE LIMIT EXCEEEDED\n");
              
              /*lock the main queue mutex*/
              pthread_mutex_lock(&main_log_queue_mutex);
    
              /*send the message to the queue and check for success*/
              if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
              {
                printf("\nERROR: mqsend\n");
                exit(1);
              }
              else if(main_log_queue_count < 10) /*check if messages on queue does not exceed*/
              {
                main_log_queue_count++;
                printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
              }
    
              /*unlock the main queue mutex*/
              pthread_mutex_unlock(&main_log_queue_mutex);
 
            }
          }
        }
        else if(receiver.task_ID == 3)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
        
            /*convert the data from string to INT*/
            int value = atoi(receiver.message);
            
            /*check for condition*/
            if(value < 50 && value > 25)
            {
              time_t a = time(NULL);
              maintask.current_time = ctime(&a);
              maintask.task_ID = decision_task;
              maintask.logged_level = ALERT;
              strcpy(maintask.message_string,"ALERT!!! ALERT!!! ALERT!!! LIGHT VALUE HAS EXCEEDED LIMIT");
              sprintf(maintask.message,"%d",value);
              maintask.message_length = strlen(maintask.message);  
              
              /*lock the main queue mutex*/
              pthread_mutex_lock(&main_log_queue_mutex);
    
              /*send the message to the queue and check for success*/
              if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
              {
                printf("\nERROR: mqsend\n");
                exit(1);
              }
              else if(main_log_queue_count < 10) /*check if messages on queue does not exceed*/
              {
                main_log_queue_count++;
                printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
              }
    
              /*unlock the main queue mutex*/
              pthread_mutex_unlock(&main_log_queue_mutex);
 
              printf("\nALERT!!!! ALERT!!!! ALERT!!!!! LIGHTTT LIMIT EXCEEEDED\n");
            }
          }
        }
        else if(receiver.task_ID == 4)
        {
          printf("\nTASK LOGGER. NO DECISION TO BE MADE\n");
        }
  
        /*get attributes to check for remaining messages on queue*/
        mq_getattr(decision_mqdes1,&attr);
      }
      
      /*reset the queue count*/
      decision_queue_count = 0;
    }
    
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&decision_queue_mutex);
    usleep(150000);
  }
  
  /*exit the thread*/
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 4\n");
  pthread_exit(NULL);

}


