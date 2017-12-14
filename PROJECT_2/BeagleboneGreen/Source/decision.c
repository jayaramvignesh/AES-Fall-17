/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: decision.c
*
*   description:
*
*   -source file for decision thread
*
*   -this thread sends a signal to release the main thread cond2
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
#include "BBG_LED.h"
#include "ERROR_LIB.h"

#define ACCELEROMETER_THRESHOLD 90.000000
#define ULTRASONIC_THRESHOLD 5
#define ALCOHOL_THRESHOLD 60

/*function for decision thread*/
void *decision_function()
{
  while(1)
  {
 
    /*send condition signal to main indicating alive*/
    pthread_cond_broadcast(&main_thread2_cond); 

    /*check for graceful exit*/
    if(exit_flag == 1)
    {
        break;
    }

    char buffer[MSG_SIZE];
    
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&socket_decision_queue_mutex);
    
    /*get the attributes for the queue*/
    mq_getattr(socket_decision_mqdes1,&attr);
    
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
        int n = mq_receive(socket_decision_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        /*check for which task*/
        if(receiver.task_ID == accelerometer_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            char *ptr = ctime(&a);
            float data= atof(receiver.message);
            if(data == ACCELEROMETER_THRESHOLD)
            {
              strcpy(decisiontask.current_time,ptr); 
              decisiontask.task_ID = decision_task;
              decisiontask.logged_level = ALERT;
              strcpy(decisiontask.message_string,"ALERT!!! ACCELEROMETER ACTIVE");
              strcpy(decisiontask.message,receiver.message);
              decisiontask.message_length = strlen(decisiontask.message);  
              LEDOn(3); 
              printf("\nALERT!! ACCELEROMETER ACTVE\n");
              /*lock the main queue mutex*/
              pthread_mutex_lock(&decision_log_queue_mutex);
    
              /*send the message to the queue and check for success*/
              if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
              {
                printf("\nERROR: mqsend\n");
                exit(1);
              }
              
              /*unlock the main queue mutex*/
              pthread_mutex_unlock(&decision_log_queue_mutex);
            }
            else
            {
              LEDOff(3);
            }

         }
        }
        else if(receiver.task_ID == alcohol_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            char *ptr = ctime(&a);
            strcpy(decisiontask.current_time,ptr); 
            decisiontask.task_ID = decision_task;
            float data = atof(receiver.message);
            if(data > ALCOHOL_THRESHOLD)
            {
              decisiontask.logged_level = ALERT;
              strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! ALCOHOL DETECTED. HE IS DRUNKK!!!!");
              strcpy(decisiontask.message,receiver.message);
              decisiontask.message_length = strlen(decisiontask.message);  
              printf("\nALERT!!! ALERT!!! ALERT!!! ALCOHOL DETECTED. HE IS DRUNKK!!!!\n");
              /*lock the main queue mutex*/
              pthread_mutex_lock(&decision_log_queue_mutex);
              LEDOn(1);
              /*send the message to the queue and check for success*/
              if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
              {
                printf("\nERROR: mqsend\n");
                exit(1);
              }
            
              
              /*unlock the main queue mutex*/
              pthread_mutex_unlock(&decision_log_queue_mutex);
            }
            else
            {
              LEDOff(1);
            }
         }
                 
        } 
        else if(receiver.task_ID == ultrasonic_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            char *ptr = ctime(&a);
            strcpy(decisiontask.current_time,ptr); 
            decisiontask.task_ID = decision_task;
            float data  = atof(receiver.message);
            if(data < ULTRASONIC_THRESHOLD)
            {
              decisiontask.logged_level = ALERT;
              strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! COLLISION MAY TAKE PLACE.. STOP STOP!!!!");
              strcpy(decisiontask.message,receiver.message);
              decisiontask.message_length = strlen(decisiontask.message);  
              LEDOn(2);
              printf("\nALERT!!! ALERT!!! ALERT!!! COLLISION MAY TAKE PLACE.. STOP STOP!!!!\n"); 
              /*lock the main queue mutex*/
              pthread_mutex_lock(&decision_log_queue_mutex);
    
              /*send the message to the queue and check for success*/
              if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
              {
                printf("\nERROR: mqsend\n");
                exit(1);
              }
             
              /*unlock the main queue mutex*/
              pthread_mutex_unlock(&decision_log_queue_mutex);
          
            }
            else
            {
              LEDOff(2);
            }
          }
        }        
      }
 
      /*get attributes to check for remaining messages on queue*/
      mq_getattr(socket_decision_mqdes1,&attr);
    }
    
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&socket_decision_queue_mutex);
    usleep(15000);
  }
  
  /*exit the thread*/
  printf("\nEXITTTTTTTTTTTTT DECISION THREADDDDDDDDDDDDDD\n");
  pthread_exit(NULL);


}



