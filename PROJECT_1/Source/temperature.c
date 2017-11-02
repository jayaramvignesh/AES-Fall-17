#include "temperature.h"

/*function for thread1*/
void *temperature_function()
{
  sig_atomic_t t_c = 0;
  printf("\nI am here : Temperature Task\n");
  temp_tk.task_ID = temperature_task;
  while(1)
  {

    pthread_cond_broadcast(&main_thread1_cond);
    if(exit_flag == 1)
    {
      break;
    }
    
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

    mq_getattr(tr_mqdes1,&attr);

    if(attr.mq_curmsgs != 0)
    {
      while(attr.mq_curmsgs != 0)
      {
       
        char source_req[100];
        char request[100];
        r_log receiver;
        int n = mq_receive(tr_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        if(receiver.Destination_task_ID == 2)
        {
          if(receiver.Source_task_ID == 1)
          {
            strcpy(source_req, "MAIN TASK");
          }
          else if(receiver.Source_task_ID == 3)
          {
            strcpy(source_req, "LIGHT TASK");
          }

          printf("\n\n\nREQUEST SENDER: %s\n\n\n",source_req);

          if(receiver.command == 'c' || receiver.command == 'C' )
          {
            printf("\n\n\nTEMPERATURE in CELSIUS\n\n\n");
            strcpy(request,"TEMPERATURE IN CELSIUS");
            int data = rand()%100;
            sprintf(temp_tk.message,"%d",data);
          }
          else if(receiver.command == 'k' || receiver.command == 'K' )
          {
            printf("\n\n\nTEMPERATURE in KELVIN\n\n\n");
            strcpy(request,"TEMPERATURE IN KELVIN");
            int data = rand()%100;
            sprintf(temp_tk.message,"%d",data);
          }
          else if(receiver.command == 'f' || receiver.command == 'F' )
          {
            printf("\n\n\nTEMPERATURE in FAHRENHEIT\n\n\n");
            strcpy(request,"TEMPERATURE IN FAHRENHEIT");
            int data = rand()%100;
            sprintf(temp_tk.message,"%d",data);
          }
          else if(receiver.command == 'd' || receiver.command == 'D')
          {
            printf("\n\n\nTEMPERATURE DATA SHOULD BE COLLECTED IN EVERY %d milliseconds\n\n\n",receiver.delay);
            sprintf(request,"TEMPERATURE DATA SHOULD BE COLLECTED IN EVERY %d milliseconds", ((receiver.delay*USEC_VALUE)/1000));
            temp_task_period = receiver.delay;
            strcpy(temp_tk.message, "-");
          }
      
          time_t a = time(NULL);
          temp_tk.current_time = ctime(&a);
          temp_tk.logged_level = SENSOR_DATA;
          sprintf(temp_tk.message_string,"REQUEST FROM %s: %s",source_req,request);
          temp_tk.message_length = strlen(temp_tk.message);
  
        }
        
        mq_getattr(tr_mqdes1,&attr);
      }
    }
    else
    {
      time_t a = time(NULL);
      temp_tk.current_time = ctime(&a);
      temp_tk.logged_level = SENSOR_DATA;
      int data = rand()%100;
      strcpy(temp_tk.message_string,"TEMPERATURE IN CELSIUS");
      sprintf(temp_tk.message,"%d",data);
      temp_tk.message_length = strlen(temp_tk.message);  
   }

    /*lock the mutex and wait for timer to fire*/
      pthread_mutex_lock(&temp_log_queue_mutex);
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

      pthread_mutex_lock(&decision_queue_mutex);
      
      /*send the message to the queue and check for success*/
      if(temp_tk.logged_level == SENSOR_DATA)
      {
        if(mq_send(decision_mqdes1,(const char *)&temp_tk, sizeof(temp_tk),0) == -1)
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

  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 1\n");
  pthread_exit(NULL);
}
