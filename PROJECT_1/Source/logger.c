/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: logger.c
*
*   description:
*
*   -source file for logger thread
*
*   -this thread sends a signal to release the main thread cond24
*   and to indicate it is alive. This is done at start of while 1
*
*   -exit flag is checked for pthread_exit
*
*   -gets messages from every queue and writes them to log file
*
*
*****************************************************************************/




#include "logger.h"

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
    ret_value = pthread_mutex_trylock(&temp_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      if(temp_log_queue_count < 11 &&  temp_log_queue_count > 0)
      {

        mq_getattr(temp_log_mqdes1,&attr);
    
        /*check if there is any messages in the queue*/
        if(attr.mq_curmsgs != 0)
        {
          char logger_level_string[200];
          char task_name[200];
          m_log receiver; 

          /*receive all the messages from queue*/
          while(attr.mq_curmsgs != 0)
          {
            Msg_no++;
      
            int n = mq_receive(temp_log_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
      
            /*check for which task*/
            if(receiver.task_ID == 1)
            {
              strcpy(task_name,"Main");
            }
            else if(receiver.task_ID == 2)
            {
              strcpy(task_name,"Temperature");
            }
            else if(receiver.task_ID == 3)
            {
              strcpy(task_name,"Light");
            }
            else if(receiver.task_ID == 4)
            {
              strcpy(task_name,"Logger");
            }
            else if(receiver.task_ID == 5)
            {
              strcpy(task_name, "Decision");
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
            else if(receiver.logged_level == 4)
            {
              strcpy(logger_level_string, "ALERT");
            }
           
            /*content for logger file*/
            sprintf(buffer,"MESSAGE NUMBER %d\n\ntask:%s\nmessage:%s\ndata:%s\nlog_level:%s\ntimestamp:%s\n",Msg_no,task_name,receiver.message_string,receiver.message,logger_level_string,receiver.current_time);
    
            /*open the logger file*/
            FILE *fd = fopen(final_file,"a+");
      
            if(fd == NULL)
            {
              printf("ERROR: NO FILE");
            }
            
            /*write to logger file*/
            fwrite(buffer,1,strlen(buffer),fd);

            /*close the file*/
            fclose(fd);

            /*reset buffer*/
            memset(buffer,'\0',sizeof(buffer));

            /*get attributes to check for remaining messages on queue*/
            mq_getattr(temp_log_mqdes1,&attr);
          }

          /*reset queue count*/
          temp_log_queue_count = 0;
        }
      }
      
      /*unclock mutex*/
      pthread_mutex_unlock(&temp_log_queue_mutex);
    }

    /*try to lock the mutex*/
    ret_value = pthread_mutex_trylock(&light_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      if(light_log_queue_count < 11 &&  light_log_queue_count > 0)
      {

        mq_getattr(light_log_mqdes1,&attr);
    
        /*check if there is any messages in the queue*/
        if(attr.mq_curmsgs != 0)
        {
          char logger_level_string[200];
          char task_name[200];
          m_log receiver; 
          while(attr.mq_curmsgs != 0)
          {
            Msg_no++;
      
            int n = mq_receive(light_log_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
      
            /*check for which task*/
            if(receiver.task_ID == 1)
            {
              strcpy(task_name,"Main");
            }
            else if(receiver.task_ID == 2)
            {
              strcpy(task_name,"Temperature");
            }
            else if(receiver.task_ID == 3)
            {
              strcpy(task_name,"Light");
            }
            else if(receiver.task_ID == 4)
            {
              strcpy(task_name,"Logger");
            }
            else if(receiver.task_ID == 5)
            {
              strcpy(task_name, "Decision");
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
            else if(receiver.logged_level == 4)
            {
              strcpy(logger_level_string, "ALERT");
            }
           
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


            mq_getattr(light_log_mqdes1,&attr);
          }

          /*reset the count*/
          light_log_queue_count = 0;
        }
      }
      
      /*unlock the mutex*/
      pthread_mutex_unlock(&light_log_queue_mutex);
    
    }
    
    /*try to lock the mutex*/
    ret_value = pthread_mutex_trylock(&main_log_queue_mutex);
    
    /*if successful, then get messages*/
    if(ret_value == 0)
    {
      /*get the attributes for the queue*/
      if(main_log_queue_count < 11 &&  main_log_queue_count > 0)
      {

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
      
            /*check for which task*/
            if(receiver.task_ID == 1)
            {
              strcpy(task_name,"Main");
            }
            else if(receiver.task_ID == 2)
            {
              strcpy(task_name,"Temperature");
            }
            else if(receiver.task_ID == 3)
            {
              strcpy(task_name,"Light");
            }
            else if(receiver.task_ID == 4)
            {
              strcpy(task_name,"Logger");
            }
            else if(receiver.task_ID == 5)
            {
              strcpy(task_name, "Decision");
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
            else if(receiver.logged_level == 4)
            {
              strcpy(logger_level_string, "ALERT");
            }
           
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


            mq_getattr(main_log_mqdes1,&attr);
          }

          /*reset count*/
          main_log_queue_count = 0;
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
   
    /*sleep*/
    usleep(15000);
  }
  
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 3\n");
  pthread_exit(NULL);
}



