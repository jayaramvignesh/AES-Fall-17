#include "light.h"

/*function for light thread*/
void *light_function()
{

  sig_atomic_t l_c = 0;
  printf("\nI am here : Light Task\n");
  light_tk.task_ID = light_task;
  while(1)
  {
    pthread_cond_broadcast(&main_thread2_cond);
    l_c++;
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
    
    if(exit_flag == 1)
    {
      break;
    }
    
    mq_getattr(lr_mqdes1,&attr);

    if(attr.mq_curmsgs != 0)
    {
      while(attr.mq_curmsgs != 0)
      {
       
        char source_req[100];
        char request[100];
        r_log receiver;
        int n = mq_receive(lr_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        if(receiver.Destination_task_ID == 3)
        {
          if(receiver.Source_task_ID == 1)
          {
            strcpy(source_req, "MAIN TASK");
          }
          else if(receiver.Source_task_ID == 2)
          {
            strcpy(source_req, "TEMPEREATURE TASK");
          }

          printf("\n\n\nREQUEST SENDER: %s\n\n\n",source_req);

          if(receiver.command == 'l' || receiver.command == 'L' )
          {
            printf("\n\n\nLIGHT DATA\n\n\n");
            strcpy(request,"LIGHT DATA");
            int data = rand()%100;
            sprintf(light_tk.message,"%d",data);

          }
          else if(receiver.command == 'd' || receiver.command == 'D')
          {
            printf("\n\n\nLIGHT DATA SHOULD BE COLLECTED IN EVERY %d milliseconds\n\n\n",receiver.delay);
            sprintf(request,"LIGHT DATA SHOULD BE COLLECTED IN EVERY %d milliseconds", ((receiver.delay*USEC_VALUE)/1000));
            light_task_period = receiver.delay;
            strcpy(light_tk.message, "-");
          }

          time_t a = time(NULL);
          light_tk.current_time = ctime(&a);
          light_tk.logged_level = SENSOR_DATA;
          sprintf(light_tk.message_string, "REQUEST FROM %s: %s",source_req,request);
          light_tk.message_length = strlen(light_tk.message);

        }
        
        mq_getattr(lr_mqdes1,&attr);
      }
    }
    else
    {
      time_t a = time(NULL);
      light_tk.current_time = ctime(&a);
      light_tk.logged_level = SENSOR_DATA;
      strcpy(light_tk.message_string, "LUMINOSITY VALUE");
      int data = rand()%100;
      sprintf(light_tk.message,"%d",data);
      light_tk.message_length = strlen(light_tk.message);
     }

     /*lock the mutex and wait for timer to fire*/
      pthread_mutex_lock(&light_log_queue_mutex);
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

  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 2\n");
  pthread_exit(NULL);
}




