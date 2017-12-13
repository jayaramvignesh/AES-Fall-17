/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: logger.c
*
*   description:
*
*   -- logger_function():
*
*     -source file for logger thread
*
*     -this thread sends a signal to release the main thread cond24
*     and to indicate it is alive. This is done at start of while 1
*
*     -exit flag is checked for pthread_exit
*
*     -gets messages from every queue and writes them to log file
*
*   --logger_id_decode
*     -This function checks for the log level of the received packet
*
*   --logger_task_name
*     -This function checks for the task id for the received packet
*
*****************************************************************************/



#include "logger.h"

/*function to decode the log id*/
void logger_id_decode(char logger_level_string[], m_log receiver)
{
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
  else if(receiver.logged_level == 4)
  {
    strcpy(logger_level_string, "ALERT");
  }
           

}

/*functoin to decode the task id*/
void logger_task_name(char task_name[], m_log receiver)
{
  /*check for which task*/
 
  if(receiver.task_ID == 1)
  {
    strcpy(task_name,"Main TIVA");  
  }          
  else if(receiver.task_ID == 2)
  {
    strcpy(task_name,"Socket task TIVA");
  }
  else if(receiver.task_ID == 3)
  {
    strcpy(task_name,"Accelerometer");  
  }
  else if(receiver.task_ID == 4)
  {
    strcpy(task_name,"Alcohol"); 
  }
  else if(receiver.task_ID == 5)
  {
    strcpy(task_name, "Carbon Monooxide");
  }
  else if(receiver.task_ID == 6)
  {
    strcpy(task_name, "Ultrasonic sensor");  
  }
  else if(receiver.task_ID == 7)
  {
    strcpy(task_name, "Main BBG");
  }
  else if(receiver.task_ID == 8)
  {
    strcpy(task_name, "Socket task BBG");  
  }
  else if(receiver.task_ID == 9)
  {
    strcpy(task_name, "Decision task");
  }           
  else if(receiver.task_ID == 10)
  {
    strcpy(task_name, "Logger task");
  }

}

/*function for logger thread */
void *logger_function()
{
  static int Msg_no;
  int ret_value = 0;
  printf("\nI am here : Logger Task\n");
  while(1)
  {  
    /*send condition signal to main indicating alive*/
    pthread_cond_broadcast(&main_thread3_cond);

    char buffer[MSG_SIZE];
    
   /*try to lock the mutex*/
    ret_value = pthread_mutex_trylock(&main_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      
      mq_getattr(main_log_mqdes1,&attr);
    
      /*check if there is any messages in the queue*/
      if(attr.mq_curmsgs != 0)
      {
        char logger_level_string[200]; 
        char task_name[200];
        m_log receiver; 
        while(attr.mq_curmsgs != 0)
        {
          Msg_no++;

          int n = mq_receive(main_log_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
    
          logger_task_name(task_name, receiver);
          logger_id_decode(logger_level_string, receiver);

          /*content for logger file*/
          sprintf(buffer,"\nMESSAGE NUMBER %d\n\ntask:%s\nmessage:%s\ndata:%s\nlog_level:%s\ntimestamp:%s\n",Msg_no,task_name,receiver.message_string,receiver.message,logger_level_string,receiver.current_time);
    
          /*open the logger file*/
          FILE *fd = fopen(final_file,"a+");
      
          if(fd == NULL)
          {
            printf("ERROR: NO FILE");
          }
          
          /*write to file*/
          fwrite(buffer,1,strlen(buffer),fd);

          /*close the file*/
          fclose(fd);
          
          /*reset buffer*/
          memset(buffer,'\0',sizeof(buffer));
          mq_getattr(main_log_mqdes1,&attr); 
    
        }  
      }
      
      /*unlock mutex*/
      pthread_mutex_unlock(&main_log_queue_mutex);
    }
  
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      /*send condition signal to main indicating alive*/
      pthread_cond_broadcast(&main_thread3_cond);
      break;
    }
  
    /*try to lock the mutex*/
    ret_value = pthread_mutex_trylock(&socket_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      
      mq_getattr(socket_log_mqdes1,&attr);
    
      /*check if there is any messages in the queue*/
      if(attr.mq_curmsgs != 0)
      {
        char logger_level_string[200]; 
        char task_name[200];
        m_log receiver; 
        while(attr.mq_curmsgs != 0)
        {
          Msg_no++;

          int n = mq_receive(socket_log_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
    
          logger_task_name(task_name, receiver);
          logger_id_decode(logger_level_string, receiver);

          /*content for logger file*/
          sprintf(buffer,"MESSAGE NUMBER %d\n\ntask:%s\nmessage:%s\ndata:%s\nlog_level:%s\ntimestamp:%s\n",Msg_no,task_name,receiver.message_string,receiver.message,logger_level_string,receiver.current_time);
    
          /*open the logger file*/
          FILE *fd = fopen(final_file,"a+");
      
          if(fd == NULL)
          {
            printf("ERROR: NO FILE");
          }
          
          /*write to file*/
          fwrite(buffer,1,strlen(buffer),fd);

          /*close the file*/
          fclose(fd);
          
          /*reset buffer*/
          memset(buffer,'\0',sizeof(buffer));
          mq_getattr(socket_log_mqdes1,&attr); 
    
        }  
      }
      
      /*unlock mutex*/
      pthread_mutex_unlock(&socket_log_queue_mutex);
    }
  
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      /*send condition signal to main indicating alive*/
      pthread_cond_broadcast(&main_thread3_cond);
      break;
    }

    /*try to lock the mutex*/
    ret_value = pthread_mutex_trylock(&decision_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      
      mq_getattr(decision_log_mqdes1,&attr);
    
      /*check if there is any messages in the queue*/
      if(attr.mq_curmsgs != 0)
      {
        char logger_level_string[200]; 
        char task_name[200];
        m_log receiver; 
        while(attr.mq_curmsgs != 0)
        {
          Msg_no++;

          int n = mq_receive(decision_log_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
    
          logger_task_name(task_name, receiver);
          logger_id_decode(logger_level_string, receiver);

          /*content for logger file*/
          sprintf(buffer,"MESSAGE NUMBER %d\n\ntask:%s\nmessage:%s\ndata:%s\nlog_level:%s\ntimestamp:%s\n",Msg_no,task_name,receiver.message_string,receiver.message,logger_level_string,receiver.current_time);
    
          /*open the logger file*/
          FILE *fd = fopen(final_file,"a+");
      
          if(fd == NULL)
          {
            printf("ERROR: NO FILE");
          }
          
          /*write to file*/
          fwrite(buffer,1,strlen(buffer),fd);

          /*close the file*/
          fclose(fd);
          
          /*reset buffer*/
          memset(buffer,'\0',sizeof(buffer));
          mq_getattr(decision_log_mqdes1,&attr); 
    
        }  
      }
      
      /*unlock mutex*/
      pthread_mutex_unlock(&decision_log_queue_mutex);
    }
  
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      /*send condition signal to main indicating alive*/
      pthread_cond_broadcast(&main_thread3_cond);
      break;
    }
       
    /*sleep*/
    usleep(15000);
  
  }
  
  printf("\nEXITTTTTTTTTTTTT LOGGGGERRR THREADDDDDDDDDDDDDD\n");
  pthread_exit(NULL);

}



