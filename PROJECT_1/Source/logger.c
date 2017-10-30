#include "logger.h"

/*function for thread 3*/
void *logger_function()
{
  while(1)
  {
   
    pthread_cond_broadcast(&main_thread3_cond);
    char buffer[MSG_SIZE];
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread3_mutex);
    pthread_cond_wait(&thread3_cond,&thread3_mutex); 

    printf("\nInside thread 3\n");  
    /*get the attributes for the queue*/
    mq_getattr(mqdes1,&attr);
    
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
      
        int n = mq_receive(mqdes1,(char*)&receiver,MSG_SIZE,NULL);
      
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
           
        /*content for logger file*/
        sprintf(buffer,"MESSAGE NUMBER %d\n\ntask:%s\ndata:%s\nlog_level:%s\ntimestamp:%s\n",Msg_no,task_name,receiver.message,logger_level_string,receiver.current_time);
    
        /*open the logger file*/
        FILE *fd = fopen(final_file,"a+");
      
        if(fd == NULL)
        {
          printf("ERROR: NO FILE");
        }
        fwrite(buffer,1,strlen(buffer),fd);
        fclose(fd);
      
        if(n != -1)
        {
          printf("\ntimestamp is %smessage is %s\nmessage length is %d\n",receiver.current_time,receiver.message,receiver.message_length);
        }
        
        mq_getattr(mqdes1,&attr);
      }

    }
    
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread3_mutex);
 
    if(exit_flag == 1)
    {
        break;
    }

 
    count = 0;

  }
  
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 3\n");
  pthread_exit(NULL);

}


