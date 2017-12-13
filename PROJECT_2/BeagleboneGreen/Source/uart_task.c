/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 13th Dec  2017
*
*   file: uart_task.c
*
*   description:
*
*   -source file for uart thread
*
*   -this thread sends a signal to release the main thread cond1
*   and to indicate it is alive. This is done at start of while 1
*
*   -exit flag is checked for pthread_exit
*
*   -this tasks receives data from tiva from UART and adds them to the 
*    logger and decision queues
*       
*****************************************************************************/



#include "uart_task.h"
#include "bbg_uart.h"
#include "logger.h"

/*function to decode the log id*/
void log_id_decode(char logger_level_string[], uart_packet receiver)
{
  /*check logger level*/
  if(receiver.log_level == 1)
  {
    strcpy(logger_level_string,"SENSOR_DATA");
  }
  else if(receiver.log_level == 2)
  {
    strcpy(logger_level_string,"ERROR");
  }
  else if(receiver.log_level == 3)
  {
    strcpy(logger_level_string,"INFO");
  }
  else if(receiver.log_level == 4)
  {
    strcpy(logger_level_string, "ALERT");
  }
}

/*function to decode the task id*/
void log_task_name(char task_name[], uart_packet receiver)
{
  /*check for which task*/
 
  if(receiver.log_id == 1)
  {
    strcpy(task_name,"Main TIVA");  
  }          
  else if(receiver.log_id == 2)
  {
    strcpy(task_name,"Socket task TIVA");
  }
  else if(receiver.log_id == 3)
  {
    strcpy(task_name,"Accelerometer");  
  }
  else if(receiver.log_id == 4)
  {
    strcpy(task_name,"Alcohol"); 
  }
  else if(receiver.log_id == 5)
  {
    strcpy(task_name, "Carbon Monooxide");
  }
  else if(receiver.log_id == 6)
  {
    strcpy(task_name, "Ultrasonic sensor");  
  }
  else if(receiver.log_id == 7)
  {
    strcpy(task_name, "Main BBG");
  }
  else if(receiver.log_id == 8)
  {
    strcpy(task_name, "Socket task BBG");  
  }
  else if(receiver.log_id == 9)
  {
    strcpy(task_name, "Decision task");
  }           
  else if(receiver.log_id == 10)
  {
    strcpy(task_name, "Logger task");
  }

}


/*function for socket thread*/
void *socket_function()
{

  int n = 0;
  
  /*initializing uart*/
  uart_init();
  while(1)
  {
    /*instance of uart packet */
    uart_packet rec;
    m_log receive;
    pthread_cond_broadcast(&main_thread1_cond);
    
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      break;
    }
    
  
    /*lock the main queue mutex*/
    pthread_mutex_lock(&socket_log_queue_mutex);
   
    memset(&rec,'\0',sizeof(rec));
    
    /*Waiting to receive the thread*/
    printf("\nWAITING TO RECEIVE THE DATA FROM TIVA UART\n");
    
    n=read(fd,&rec,sizeof(rec));
    
    /*check if data is received*/
    if(n <= 0)
    {
      printf("\nRead Fail\n");
    }
    else
    {
      receive.logged_level = rec.log_level;

      /*check the received log level*/
      if(receive.logged_level == SENSOR_DATA)
      {
        sprintf(receive.message,"%f",rec.data);
        strcpy(receive.message_string, "Data value of ");  
      }
      else
      {
        strcpy(receive.message_string, "Message of ");
        strcpy(receive.message, "-");
      }
    
      /*create the packet for logging*/
      receive.task_ID = rec.log_id;
      strcpy(receive.current_time,rec.timestamp);
      char temp[20] = {};
      log_task_name(temp,rec);
      strcat(receive.message_string,temp);
      printf("\n%s\n",receive.message_string);
      printf("\ndata is %s\n",receive.message);
      printf("\nReceived Time Stamp is %s\n",receive.current_time);
      
      /*send the message to the queue and check for success*/
      if(mq_send(socket_log_mqdes1,(const char *)&receive, sizeof(receive),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
  
      /*unlock the main queue mutex*/
      pthread_mutex_unlock(&socket_log_queue_mutex);
  
      /*if received data is sensor data, then only add to decision queue*/
      if(receive.logged_level == SENSOR_DATA)
      {
        /*send the message to the queue and check for success*/
        if(mq_send(socket_decision_mqdes1,(const char *)&receive, sizeof(receive),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
      }

    }
    
    usleep(1000);
  }

  printf("\nEXITTTTTTTT SOCKET/UART THREAD\n");
  close(fd);
  pthread_exit(NULL);
}


