/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: light.c
*
*   description:
*
*   -source file for light thread
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



#include "light.h"

/*function for light thread*/
void *light_function()
{

  sig_atomic_t l_c = 0;
  printf("\nI am here : Light Task\n");
  light_tk.task_ID = light_task;
  printf("\nBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");
  int file;
  file = InitLightSensor(); 
  error_t ret = readIDReg(file); 
  ret = configureSensorDefault(file); 
  float lux = readLux(file);
  printf("Returned Lux Value: %f\n", lux); 
  printf("\nBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");
  
  while(1)
  {
    /*send condition to main indicating alive*/
    pthread_cond_broadcast(&main_thread2_cond);
    
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      break;
    }

    l_c++;
    
    /*if count == 10, send async request to temperature thread*/
    if(l_c == 10)
    {
      r_log send;
      send.Destination_task_ID = 2;
      send.Source_task_ID = 3;
      send.command = 'd';
      send.delay = 10;
      if(mq_send(tr_mqdes1,(const char *)&send, sizeof(send),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
    }
    
    /*get attributes for request queue*/
    mq_getattr(lr_mqdes1,&attr);

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
        int n = mq_receive(lr_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        
        /*check of request is for light task*/ 
        if(receiver.Destination_task_ID == 3)
        {
          /*check which task sent request*/
          if(receiver.Source_task_ID == 1)
          {
            strcpy(source_req, "MAIN TASK");
          }
          else if(receiver.Source_task_ID == 2)
          {
            strcpy(source_req, "TEMPEREATURE TASK");
          }

          printf("\n\n\nREQUEST SENDER: %s\n\n\n",source_req);

          /*check for command*/
          if(receiver.command == 'l' || receiver.command == 'L' )
          {
            printf("\n\n\nLIGHT DATA\n\n\n");
            strcpy(request,"LIGHT DATA");
            float data = readLux(file);
            sprintf(light_tk.message,"%f",data);
            light_tk.logged_level = SENSOR_DATA;
          }
          else if(receiver.command == 'd' || receiver.command == 'D')
          {
            printf("\n\n\nLIGHT DATA SHOULD BE COLLECTED IN EVERY %d milliseconds\n\n\n",receiver.delay);
            sprintf(request,"LIGHT DATA SHOULD BE COLLECTED IN EVERY %d milliseconds", ((receiver.delay*USEC_VALUE)/1000));
            light_task_period = receiver.delay;
            strcpy(light_tk.message, "-");
            light_tk.logged_level = INFO;
          }

          /*add to log file*/
          time_t a = time(NULL);
          light_tk.current_time = ctime(&a);
          sprintf(light_tk.message_string, "REQUEST FROM %s: %s",source_req,request);
          light_tk.message_length = strlen(light_tk.message);

        }
        
        /*get attribute to check for remaining request*/
        mq_getattr(lr_mqdes1,&attr);
      }
    }
    else
    {
      /*get the light data*/
      time_t a = time(NULL);
      light_tk.current_time = ctime(&a);
      light_tk.logged_level = SENSOR_DATA;
      strcpy(light_tk.message_string, "LUMINOSITY VALUE"); 
      float data = readLux(file);
      sprintf(light_tk.message,"%f",data);
      light_tk.message_length = strlen(light_tk.message);
     }

      /*lock the mutex and wait for timer to fire*/
      pthread_mutex_lock(&light_log_queue_mutex);


      /*wait for timer to fire*/
      pthread_cond_wait(&light_task_cond,&light_log_queue_mutex); 
    
      printf("\nInside light thread\n");  
    
      /*send the message to the queue and check for success*/
      if(mq_send(light_log_mqdes1,(const char *)&light_tk, sizeof(light_tk),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
      else if(light_log_queue_count < 10)
      {
        light_log_queue_count++;
        printf("\nLIGHT MESSAGE SENT.. LOG QUEUE COUNT is %d\n",light_log_queue_count);
      }
    
      /*unlock the mutex and exit*/
      pthread_mutex_unlock(&light_log_queue_mutex);

      /*check for graceful exit*/
      if(exit_flag == 1)
      {
        break;
      }

      pthread_mutex_lock(&decision_queue_mutex);
      
      /*send the message to the queue and check for success*/
      if(temp_tk.logged_level == SENSOR_DATA)
      {
        if(mq_send(decision_mqdes1,(const char *)&light_tk, sizeof(light_tk),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(decision_queue_count < 10)
        {
          decision_queue_count++;
          printf("\n DECISION QUEUE COUNT is %d\n",decision_queue_count);
        }
      }

      pthread_mutex_unlock(&decision_queue_mutex);

  }

  /*kill thread*/
  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 2\n");
  pthread_exit(NULL);
}




