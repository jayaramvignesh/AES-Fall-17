/****************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 14th Dec 2017
*
*   file: main.c
*
*   description:
*   main source file
*   - creates threads
*   - handles heartbeats
*   - destroys mutex and other variables on exit
*
*****************************************************************************/


#include "BBG_LED.h"
#include "main.h"
#include "sig.h"
#include "timer.h"
#include "uart_task.h"
#include "logger.h"
#include "decision.h"
#include "bbg_uart.h"

int main(int argc, char* argv[])
{
  char buffer[MSG_SIZE];
  
  /*set default file name and path to file*/
  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  
  strcpy(path_name,path); 
  
  if(argc < 1)
  {
    printf("\nERROR:NO ARGUMENTS PASSED.\n");
    exit(1);

  }
  else if(argc == 3)
  {
    strcpy(file_name,argv[1]);
    strcpy(path_name, argv[2]);
  }
  else if(argc == 2)
  {
    strcpy(file_name,argv[1]);
  }
  else if(argc > 3)
  {
    printf("\nTOOOOOOO MANY ARGUMENTS PASSED\n");
  }

  strcat(path_name,"/");
  strcat(final_file,path_name);
  strcat(final_file,file_name);
  printf("\nfile name is %s\n",file_name);
  printf("\npath name is %s\n",path_name);
  printf("\nFINAL is %s\n",final_file);

  /*GEt PID of current process*/
  pid_t pi;
  pi = getpid();
  printf("PIDD is %d\n",pi);

  LEDOff(0);
  LEDOff(1);
  LEDOff(2);
  LEDOff(3);
  /*if file already exists, remove it*/
  remove(file_name);
  
  /*setting attributes for the message queue*/
  attr.mq_maxmsg = NO_OF_MGS;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(MAIN_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  main_log_mqdes1=  mq_open(MAIN_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(main_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*removes the queue name if already existing*/
  mq_unlink(SOCKET_LOG_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  socket_log_mqdes1=  mq_open(SOCKET_LOG_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(socket_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }


  /*removes the queue name if already existing*/
  mq_unlink(SOCKET_DECISION_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  socket_decision_mqdes1=  mq_open(SOCKET_DECISION_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(socket_decision_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*removes the queue name if already existing*/
  mq_unlink(DECISION_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  decision_log_mqdes1=  mq_open(DECISION_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(decision_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*Setup sigint for graceful exit*/
  setup_SIGINT();
  
  /*initialize mutex for main log  queue and check for return*/
  if(pthread_mutex_init(&main_log_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }

  /*initialize mutex for socket log  queue and check for return*/
  if(pthread_mutex_init(&socket_log_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }

  /*initialize mutex for socket decision queue and check for return*/
  if(pthread_mutex_init(&socket_decision_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }
  
  /*initialize mutex for decision log  queue and check for return*/
  if(pthread_mutex_init(&decision_log_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }

  /*initialize mutex for heartbeat of thread 1  and check for return*/
  if(pthread_mutex_init(&main_thread1_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }


  /*initialize condition variable for heartbeat of thread 1 and check for return*/
  if(pthread_cond_init(&main_thread1_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return PTHREAD_COND_INIT_FAIL;
  }

  /*initialize mutex  for heartbeat of thread 2 and check for return*/
  if(pthread_mutex_init(&main_thread2_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }

  /*initialize condition variable for heartbeat of thread 2  and check for return*/
  if(pthread_cond_init(&main_thread2_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return PTHREAD_COND_INIT_FAIL;
  }

  /*initialize mutex for heartbeat of thread 3  and check for return*/
  if(pthread_mutex_init(&main_thread3_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return PTHREAD_MUTEX_INIT_FAIL;
  }

  /*initialize condition variable for heartbeat of thread 3  and check for return*/
  if(pthread_cond_init(&main_thread3_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return PTHREAD_COND_INIT_FAIL;
  }

  pthread_t socket_thread,decision_thread,logger_thread;
  
  int return_value = pthread_create(&socket_thread,NULL,&socket_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return PTHREAD_CREATE_FAIL;
  }
  else
  {
    /*if successful creation, log it*/
    time_t a = time(NULL);
    char *ptr = ctime(&a);
    strcpy(maintask.current_time,ptr); 
    maintask.logged_level = INFO;
    maintask.task_ID = main_bbg_task;
    strcpy(maintask.message_string, "SOCKET TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    /*lock the main queue mutex*/
    pthread_mutex_lock(&main_log_queue_mutex);
    
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
      exit(1);
    }

    /*unlock the main queue mutex*/
    pthread_mutex_unlock(&main_log_queue_mutex);
  }  

  /*create thread for logger task and check for return*/
  return_value = pthread_create(&logger_thread,NULL,&logger_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return PTHREAD_CREATE_FAIL;
  }
  else
  {
    /*if successful creation, log it*/
    time_t a = time(NULL);
    char *ptr = ctime(&a);
    strcpy(maintask.current_time,ptr); 
    maintask.logged_level = INFO;
    maintask.task_ID = main_bbg_task;
    strcpy(maintask.message_string, "LOGGER TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    /*lock the main queue mutex*/
    pthread_mutex_lock(&main_log_queue_mutex);
   
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
   
    /*unlock the main queue mutex*/
    pthread_mutex_unlock(&main_log_queue_mutex);

   }  

  /*create thread for decision task and check for return*/
  return_value = pthread_create(&decision_thread,NULL,&decision_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return PTHREAD_CREATE_FAIL;
  }
  else
  {
    /*if successful creation, log it*/
    time_t a = time(NULL);
    char *ptr = ctime(&a);
    strcpy(maintask.current_time,ptr); 
    maintask.logged_level = INFO;
    maintask.task_ID = main_bbg_task;
    strcpy(maintask.message_string, "DECISION TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    /*lock the main queue mutex*/
    pthread_mutex_lock(&main_log_queue_mutex);

    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }

    /*unlock the main queue mutex*/
    pthread_mutex_unlock(&main_log_queue_mutex);

   }  
  
  struct timespec ts;
  while(1)
  {
   
    /*setup heartneaet for every 2 seconds*/
    ts = heartbeat_setup(20,4000000);
        
    if(exit_flag == 1)
    {
      printf("\nBREAKING FROM MAIN\n");
      break;
    }
    else
    {
      /*check for heartbeat from temperature task using pthread_cond_timedwait*/
      pthread_mutex_lock(&main_thread1_mutex);
      return_value = pthread_cond_timedwait(&main_thread1_cond,&main_thread1_mutex,&ts); 
      pthread_mutex_unlock(&main_thread1_mutex);

      /*check the return value*/
      if(return_value == 0)
      {
//        printf("\nThread SOCKET is working\n");
      }
      else
      {
        /*if thread 1 is stuck, log it and call signal handler for graceful exit*/
        printf("\nSOCKET THREAD STUCK!!!!!!!!!!!\n");
        time_t a = time(NULL);
        char *ptr = ctime(&a);
        strcpy(maintask.current_time,ptr); 
        maintask.logged_level = ERROR;
        maintask.task_ID = main_bbg_task;
        strcpy(maintask.message_string, "SOCKET THREAD  STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*lock the main queue mutex*/
        pthread_mutex_lock(&main_log_queue_mutex);
 
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        
        /*unlock the main queue mutex*/
        pthread_mutex_unlock(&main_log_queue_mutex);
 
        /*call signal handler for graceful exit*/ 
        signal_handler();
        continue;
      }
     
      if(exit_flag == 1)
      {
        printf("\nBREAKING FROM MAIN\n");
        break;
      } 
     
      /*check for heartbeat from light task using pthread_cond_timedwait*/
      pthread_mutex_lock(&main_thread2_mutex);
      return_value = pthread_cond_timedwait(&main_thread2_cond,&main_thread2_mutex,&ts);
      pthread_mutex_unlock(&main_thread2_mutex);
      
      /*check the return value*/
      if(return_value == 0)
      {
    //    printf("\nThread DECISION is working\n");
      }
      else
      {

        /*if thread 2 is stuck, log it and call signal handler for graceful exit*/
        printf("\nTHREAD DECISION STUCK!!!!!\n");
        time_t a = time(NULL);
        char *ptr = ctime(&a);
        strcpy(maintask.current_time,ptr); 
        maintask.logged_level = ERROR;
        maintask.task_ID = main_bbg_task;
        strcpy(maintask.message_string, "THREAD DECISION STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
       
        /*lock the main queue mutex*/
        pthread_mutex_lock(&main_log_queue_mutex);
  
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
       
        /*unlock the main queue mutex*/
        pthread_mutex_unlock(&main_log_queue_mutex);
 
        /*call signal handler for graceful exit*/ 
        signal_handler();
        continue;
      }

      if(exit_flag == 1)
      {
        printf("\nBREAKING FROM MAIN\n");
        break;
      }

      /*check for heartbeat from decision task using pthread_cond_timedwait*/
      pthread_mutex_lock(&main_thread3_mutex);
      return_value = pthread_cond_timedwait(&main_thread3_cond,&main_thread3_mutex,&ts); 
      pthread_mutex_unlock(&main_thread3_mutex);
      
      /*check the return value*/
      if(return_value == 0)
      {
      //    printf("\nThread logger is working\n");;
      }      
      else
      {
        /*if thread 3 is stuck, log it and call signal handler for graceful exit*/
        printf("\nTHREAD Logger STUCK!!!!!!!!\n");
        time_t a = time(NULL);
        char *ptr = ctime(&a);
        strcpy(maintask.current_time,ptr); 
        maintask.logged_level = ERROR;
        maintask.task_ID = main_bbg_task;
        strcpy(maintask.message_string, "THREAD LOGGER STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*lock the main queue mutex*/
        pthread_mutex_lock(&main_log_queue_mutex);
 
 
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }

        /*unlock the main queue mutex*/
        pthread_mutex_unlock(&main_log_queue_mutex);
 
        /*call signal handler for graceful exit*/ 
        signal_handler();
        continue;
      }
       
      usleep(10000);
    }
  }

  pthread_join(decision_thread, NULL);
  pthread_join(socket_thread, NULL);
  pthread_join(logger_thread, NULL);
 
  printf("\nALL CHILDREN ARE DEAD !!!!\n");

  /*destroy the decision queue mutex*/
  return_value = pthread_mutex_destroy(&decision_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread DECISION MUTEX destroy: FAILED\n");
  }

  /*destroy the main queue mutex and check for return*/
  return_value = pthread_mutex_destroy(&main_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread MAIN LOG MUTEX destroy: FAILED\n");
  }

   /*destroy the socket log queue mutex and check for return*/
  return_value = pthread_mutex_destroy(&socket_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread SOCKET LOG MUTEX destroy: FAILED\n");
  }

  /*destroy the socket log queue mutex and check for return*/
  return_value = pthread_mutex_destroy(&socket_decision_queue_mutex);
  if(return_value)
  {
    printf("\nPthread SOCKET DECISION MUTEX destroy: FAILED\n");
  }

  /*destroy the main thread 1 mutex and check for return*/
  return_value = pthread_mutex_destroy(&main_thread1_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 1 destroy: FAILED\n");
  }

  /*destroy the  main thread 1 condition variable and check for return*/
  return_value =  pthread_cond_destroy(&main_thread1_cond);
  if(return_value)
  {
    printf("\nPthread main cond 1 destroy: FAILED\n");
  }

  /*destroy the  main thread 2 mutex and check for return*/
  return_value = pthread_mutex_destroy(&main_thread2_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 2 destroy: FAILED\n");
  }

  /*destroy the main thread 2  condition variable and check for return*/
  return_value =  pthread_cond_destroy(&main_thread2_cond);
  if(return_value)
  {
    printf("\nPthread main cond 2 destroy: FAILED\n");
  }

  /*destroy the main thread 3  mutex and check for return*/
  return_value = pthread_mutex_destroy(&main_thread3_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 3 destroy: FAILED\n");
  }

  /*destroy the main thread 3  condition variable and check for return*/
  return_value =  pthread_cond_destroy(&main_thread3_cond);
  if(return_value)
  {
    printf("\nPthread main cond 3 destroy: FAILED\n");
  }

  /*close main log queue*/
  mq_close(main_log_mqdes1);

  /*close decision log queue*/
  mq_close(decision_log_mqdes1);
    
  /*close socket log queue*/
  mq_close(socket_log_mqdes1);
  
  /*close socket decision queue*/
  mq_close(socket_decision_mqdes1);

  printf("\nEXIT MAIN THREAD!! BYE BYE\n");
  pthread_exit(NULL);
  
}
