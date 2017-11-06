/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: temperature.c
*
*   description:
*
*   -source file for temperature thread
*
*   -this thread sends a signal to release the main thread cond24
*   and to indicate it is alive. This is done at start of while 1
*
*   -exit flag is checked for pthread_exit
*
*   -the task sends an async request to temperature task
*
*   - task checks for requests and services them and adds them to log file
*
*   - if no requests, collects light data and adds them to log file
*
*       
*****************************************************************************/



#include "temperature.h"

/*function for thread1*/
void *temperature_function()
{
  m_log temp_decision_tk;
  sig_atomic_t t_c = 0;
  printf("\nI am here : Temperature Task\n");
  temp_decision_tk.task_ID  = temperature_task;
  temp_tk.task_ID = temperature_task;
  while(1)
  {
    /*send condition to main indicating alive*/
    pthread_cond_broadcast(&main_thread1_cond);
    
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      break;
    }
   
    /*if count == 10, send async request to light thread*/
    t_c++;
    if(t_c == 10)
    {
      r_log send;
      send.Destination_task_ID = 3;
      send.Source_task_ID = 2;
      send.command = 'd';
      send.delay = 3;
      if(mq_send(lr_mqdes1,(const char *)&send, sizeof(send),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
    }

    /*get attributes for request queue*/
    mq_getattr(tr_mqdes1,&attr);

    /*check for messages on request queue*/
    if(attr.mq_curmsgs != 0)
    {
      /*if present service all the requests*/
      while(attr.mq_curmsgs != 0)
      {
       
        char source_req[100];
        char request[100];
        r_log receiver;
        
        /*receive the request*/
        int n = mq_receive(tr_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        
        /*check of request is for temperature task*/ 
        if(receiver.Destination_task_ID == 2)
        {
          /*check which task sent request*/
          if(receiver.Source_task_ID == 1)
          {
            strcpy(source_req, "MAIN TASK");
          }
          else if(receiver.Source_task_ID == 3)
          {
            strcpy(source_req, "LIGHT TASK");
          }

          printf("\n\n\nREQUEST SENDER: %s\n\n\n",source_req);

          /*check for command*/
          if(receiver.command == 'c' || receiver.command == 'C' )
          {
            temp_tk.logged_level = SENSOR_DATA;
            temp_decision_tk.logged_level  = SENSOR_DATA;
            printf("\n\n\nTEMPERATURE in CELSIUS\n\n\n");
            strcpy(request,"TEMPERATURE IN CELSIUS");
            int c_data = rand()%100;
            sprintf(temp_tk.message,"%d",c_data);
            sprintf(temp_decision_tk.message,"%d",c_data);
          }
          else if(receiver.command == 'k' || receiver.command == 'K' )
          {
            temp_tk.logged_level = SENSOR_DATA;
            temp_decision_tk.logged_level  = SENSOR_DATA;
            printf("\n\n\nTEMPERATURE in KELVIN\n\n\n");
            strcpy(request,"TEMPERATURE IN KELVIN");
            int c_data = rand()%100;
            int k_data = rand()%100;
            sprintf(temp_tk.message,"%d",k_data);
            sprintf(temp_decision_tk.message,"%d",c_data);
          }
          else if(receiver.command == 'f' || receiver.command == 'F' )
          {
            temp_tk.logged_level = SENSOR_DATA;
            temp_decision_tk.logged_level  = SENSOR_DATA;
            printf("\n\n\nTEMPERATURE in FAHRENHEIT\n\n\n");
            strcpy(request,"TEMPERATURE IN FAHRENHEIT");
            int f_data = rand()%100;
            int c_data = rand()%100;
            sprintf(temp_tk.message,"%d",f_data);
            sprintf(temp_decision_tk.message,"%d",c_data);
          }
          else if(receiver.command == 'd' || receiver.command == 'D')
          {
            temp_tk.logged_level = INFO;
            temp_decision_tk.logged_level  = INFO;
            printf("\n\n\nTEMPERATURE DATA SHOULD BE COLLECTED IN EVERY %d milliseconds\n\n\n",receiver.delay);
            sprintf(request,"TEMPERATURE DATA SHOULD BE COLLECTED IN EVERY %d milliseconds", ((receiver.delay*USEC_VALUE)/1000));
            temp_task_period = receiver.delay;
            strcpy(temp_tk.message, "-");
            strcpy(temp_decision_tk.message, "-");
          }
     
          /*add to log file*/
          time_t a = time(NULL);
          temp_tk.current_time = ctime(&a);
          temp_decision_tk.current_time = ctime(&a);
          sprintf(temp_tk.message_string,"REQUEST FROM %s: %s",source_req,request);
          sprintf(temp_decision_tk.message_string,"REQUEST FROM %s: %s",source_req,request);
          temp_tk.message_length = strlen(temp_tk.message);
          temp_decision_tk.message_length = strlen(temp_decision_tk.message);

        }
       
        /*get attribute to check for remaining request*/
        mq_getattr(tr_mqdes1,&attr);
      }
    }
    else
    {
      /*get the temperature data*/
      time_t a = time(NULL);
      temp_tk.current_time = ctime(&a);
      temp_decision_tk.current_time = ctime(&a);
      temp_tk.logged_level = SENSOR_DATA;
      temp_decision_tk.logged_level = SENSOR_DATA;
      float data = -10.01;
      strcpy(temp_tk.message_string,"TEMPERATURE IN CELSIUS");
      strcpy(temp_decision_tk.message_string,"TEMPERATURE IN CELSIUS");
      sprintf(temp_tk.message,"%f",data);
      sprintf(temp_decision_tk.message,"%f",data);
      temp_tk.message_length = strlen(temp_tk.message);  
      temp_decision_tk.message_length = strlen(temp_tk.message);  
   }

      /*lock the mutex and wait for timer to fire*/
      pthread_mutex_lock(&temp_log_queue_mutex);
      
      /*wait for timer to fire*/
      pthread_cond_wait(&temp_task_cond,&temp_log_queue_mutex); 
     
      printf("\nInside temperature thread\n");  
    
      /*send the message to the queue and check for success*/
      if(mq_send(temp_log_mqdes1,(const char *)&temp_tk, sizeof(temp_tk),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
      else if(temp_log_queue_count < 10)
      {
        temp_log_queue_count++;
        printf("\nTEMP MESSAGE SENT.. LOG QUEUE COUNT is %d\n",temp_log_queue_count);
      }
    
      /*unlock the mutex and exit*/
      pthread_mutex_unlock(&temp_log_queue_mutex);

      /*check for graceful exit*/
      if(exit_flag == 1)
      {
        break;
      }
      
      pthread_mutex_lock(&decision_queue_mutex);
      
      /*send the message to the queue and check for success*/
      if(temp_decision_tk.logged_level == SENSOR_DATA)
      {
        if(mq_send(decision_mqdes1,(const char *)&temp_decision_tk, sizeof(temp_decision_tk),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(decision_queue_count < 10)
        {
          decision_queue_count++;
          printf("\nDECISION QUEUE COUNT is %d\n",decision_queue_count);
        }
      }

      pthread_mutex_unlock(&decision_queue_mutex);
   
  }

  /*kill thread*/
  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 1\n");
  pthread_exit(NULL);
}
