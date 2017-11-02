#include "main.h"
#include "timer.h"
#include "message.h"
#include "sig.h"
#include "temperature.h"
#include "decision.h"
#include "light.h"
#include "logger.h"


/*global variables*/
sig_atomic_t main_log_queue_count = 0;
sig_atomic_t light_log_queue_count = 0;
sig_atomic_t temp_log_queue_count = 0;
sig_atomic_t decision_queue_count = 0;
sig_atomic_t exit_flag = 0;
sig_atomic_t main_count = 0;

sig_atomic_t temp_task_period = 1;
sig_atomic_t light_task_period = 1;



int main(int argc, char* argv[])
{
  char buffer[MSG_SIZE];
  
  /*get file name and path to file*/
  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  strcpy(path_name,path);

  char c;
  while((c = getopt(argc,argv,"f:p:")) != -1)
  {
    switch(c)
    {
      case 'f':
        memset(file_name,0,100);
        strcpy(file_name,optarg);
        break;

      case 'p':
        memset(path_name,0,100);
        strcpy(path_name,optarg);
        break;

      case '?':
        printf("\nWrong arguments given!!!\n");
        exit(1);

      default:
        exit(1);
    }

  }

  strcat(path_name,"/");
  strcat(final_file,path_name);
  strcat(final_file,file_name);
  printf("\nfile name is %s\n",file_name);
  printf("\npath name is %s\n",path_name);
  printf("\nFINAL is %s\n",final_file);

  pid_t pi;
  pi = getpid();
  printf("PIDD is %d\n",pi);

  /*if file already exists, remove it*/
  remove(file_name);
  

  /*setting attributes for the message queue*/
  attr.mq_maxmsg = NO_OF_MGS;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(TEMP_LOG_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  temp_log_mqdes1=  mq_open(TEMP_LOG_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(temp_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(temp_log_mqdes1,&attr);

  /*removes the queue name if already existing*/
  mq_unlink(MAIN_LOG_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  main_log_mqdes1=  mq_open(MAIN_LOG_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(main_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(main_log_mqdes1,&attr);
 

  /*removes the queue name if already existing*/
  mq_unlink(LIGHT_LOG_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  light_log_mqdes1=  mq_open(LIGHT_LOG_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(light_log_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(light_log_mqdes1,&attr);
 
  /*removes the queue name if already existing*/
  mq_unlink(DECISION_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  decision_mqdes1=  mq_open(DECISION_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(decision_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(decision_mqdes1,&attr);
 
  /*removes the queue name if already existing*/
  mq_unlink(TEMP_REQ_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  tr_mqdes1=  mq_open(TEMP_REQ_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(tr_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(tr_mqdes1,&attr);
 
  /*removes the queue name if already existing*/
  mq_unlink(LIGHT_REQ_QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  lr_mqdes1=  mq_open(LIGHT_REQ_QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(lr_mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(lr_mqdes1,&attr);

  setup_SIGINT();

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&temp_log_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&light_log_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }


  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&decision_queue_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

 
  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&light_task_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&temp_task_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&main_thread1_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&main_thread2_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&main_thread2_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&main_thread3_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&main_thread3_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&main_thread4_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&main_thread4_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }


  pthread_t temp_thread,light_thread,logger_thread,decision_thread;
  
  int return_value = pthread_create(&temp_thread,NULL,&temperature_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return -1;
  }
  else
  {
    time_t a = time(NULL);
    maintask.current_time = ctime(&a);
    maintask.logged_level = INFO;
    maintask.task_ID = main_task;
    strcpy(maintask.message_string, "TEMPERATURE TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    pthread_mutex_lock(&main_log_queue_mutex);
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
      exit(1);
    }
    else if(main_log_queue_count < 10)
    {
      main_log_queue_count++;
      printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
    }
    pthread_mutex_unlock(&main_log_queue_mutex);
  }  

  return_value = pthread_create(&light_thread,NULL,&light_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return -1;
  }
  else
  {
    time_t a = time(NULL);
    maintask.current_time = ctime(&a);
    maintask.logged_level = INFO;
    maintask.task_ID = main_task;
    strcpy(maintask.message_string, "LIGHT TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    pthread_mutex_lock(&main_log_queue_mutex);
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
      exit(1);
    }
    else if(main_log_queue_count < 10)
    {
      main_log_queue_count++;
      printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
    }
    pthread_mutex_unlock(&main_log_queue_mutex);

   }

  return_value = pthread_create(&logger_thread,NULL,&logger_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return -1;
  }
  else
  {
    time_t a = time(NULL);
    maintask.current_time = ctime(&a);
    maintask.logged_level = INFO;
    maintask.task_ID = main_task;
    strcpy(maintask.message_string, "LOGGER TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    pthread_mutex_lock(&main_log_queue_mutex);
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
    else if(main_log_queue_count < 10)
    {
      main_log_queue_count++;
      printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
    }
    pthread_mutex_unlock(&main_log_queue_mutex);

   }  

  return_value = pthread_create(&decision_thread,NULL,&decision_function,NULL);
  if(return_value == -1)
  {
    printf("\nERROR: PTHREAD CREATE FAILED\n");
    return -1;
  }
  else
  {
    time_t a = time(NULL);
    maintask.current_time = ctime(&a);
    maintask.logged_level = INFO;
    maintask.task_ID = main_task;
    strcpy(maintask.message_string, "DECISION TASK SPAWNED!!!!!!");
    strcpy(maintask.message, "-");
    maintask.message_length = strlen(maintask.message); 
   
    pthread_mutex_lock(&main_log_queue_mutex);
    /*send the message to the queue and check for success*/
    if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
    else if(main_log_queue_count < 10)
    {
      main_log_queue_count++;
      printf("\nMESSAGE SENT. LOG QUEUE COUNT IS %d\n",main_log_queue_count);
    }
    pthread_mutex_unlock(&main_log_queue_mutex);

   }  
  /*structure for timer*/
  struct itimerval timer;
  timer = setup_timer(SEC_VALUE, USEC_VALUE);
  setitimer(ITIMER_REAL, &timer, NULL);
  
  struct timespec ts;

  while(1)
  {
    if(exit_flag == 1)
    {
      break;
    }

    main_count++;
    
    if(main_count == 10)
    {
      r_log send;
      send.Destination_task_ID = 2;
      send.Source_task_ID = 1;
      send.command = 'C';
      send.delay = 0;
      if(mq_send(tr_mqdes1,(const char *)&send, sizeof(send),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
    }
    else if(main_count == 15)
    {
      r_log send;
      send.Destination_task_ID = 3;
      send.Source_task_ID = 1;
      send.command = 'L';
      if(mq_send(lr_mqdes1,(const char *)&send, sizeof(send),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }

    }

    ts = heartbeat_setup(3,4000000);
    if(exit_flag == 1)
    {
      printf("\nBREAKING FROM MAIN\n");
      break;
    }
    else
    {
      pthread_mutex_lock(&main_thread1_mutex);
      return_value = pthread_cond_timedwait(&main_thread1_cond,&main_thread1_mutex,&ts); 
      pthread_mutex_unlock(&main_thread1_mutex);

      if(return_value == 0)
      {
        printf("\nThread one is working\n");
      }
      else
      {
        printf("\nTHREAD 1 FASSS GAYA\n");
        time_t a = time(NULL);
        maintask.current_time = ctime(&a);
        maintask.logged_level = ERROR;
        maintask.task_ID = light_task;
        strcpy(maintask.message_string, "THREAD TEMPERATURE STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(main_log_queue_count < 10)
        {
          main_log_queue_count++;
        }
        signal_handler();
        continue;
      }
      
      pthread_mutex_lock(&main_thread2_mutex);
      return_value = pthread_cond_timedwait(&main_thread2_cond,&main_thread2_mutex,&ts);
      pthread_mutex_unlock(&main_thread2_mutex);
      if(return_value == 0)
      {
        printf("\nThread 2 is working\n");
      }
      else
      {
        printf("\nTHREAD 2 FASSS GAYA\n");
        time_t a = time(NULL);
        maintask.current_time = ctime(&a);
        maintask.logged_level = ERROR;
        maintask.task_ID = light_task;
        strcpy(maintask.message_string, "THREAD LIGHT STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(main_log_queue_count < 10)
        {
          main_log_queue_count++;
        }
       
        signal_handler();
        continue;
      }


      pthread_mutex_lock(&main_thread3_mutex);
      return_value = pthread_cond_timedwait(&main_thread3_cond,&main_thread3_mutex,&ts); 
      pthread_mutex_unlock(&main_thread3_mutex);
      if(return_value == 0)
      {
          printf("\nThread three is working\n");;
      }      
      else
      {
        printf("\nTHREAD 3 FASSS GAYA\n");
        time_t a = time(NULL);

        maintask.current_time = ctime(&a);
        maintask.logged_level = ERROR;
        maintask.task_ID = light_task;
        strcpy(maintask.message_string, "THREAD LOGGER STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(main_log_queue_count < 10)
        {
          main_log_queue_count++;
        }

        signal_handler();
        continue;
      }
         
      pthread_mutex_lock(&main_thread4_mutex);
      return_value = pthread_cond_timedwait(&main_thread4_cond,&main_thread4_mutex,&ts); 
      pthread_mutex_unlock(&main_thread4_mutex);
      if(return_value == 0)
      {
        printf("\nThread four is working\n");
      }
      else
      {
        printf("\nTHREAD 2 FASSS GAYA\n");
        time_t a = time(NULL);
        maintask.current_time = ctime(&a);
        maintask.logged_level = ERROR;
        maintask.task_ID = light_task;
        strcpy(maintask.message_string, "THREAD DECISION STUCK!!!! ABORTYING EVERYTHING");
        strcpy(maintask.message, " -");
        maintask.message_length = strlen(maintask.message); 
        
        /*send the message to the queue and check for success*/
        if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        else if(main_log_queue_count < 10)
        {
          main_log_queue_count++;
        }

        signal_handler();
        continue;
      }
        
    usleep(10000);

    }
  }
  pthread_join(temp_thread, NULL);
  pthread_join(light_thread, NULL);
  pthread_join(logger_thread, NULL);
 
  printf("\nALL CHILDREN ARE DEAD !!!!\n");

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&decision_queue_mutex);
  if(return_value)
  {
    printf("\nPthread DECISION MUTEX destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&main_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread MAIN LOG MUTEX destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&temp_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread TEMP LOG MUTEX destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&temp_task_cond);
  if(return_value)
  {
    printf("\nPthread TEMP LOG COND destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&light_log_queue_mutex);
  if(return_value)
  {
    printf("\nPthread LIGHT LOG QUEUE MUTEX destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&light_task_cond);
  if(return_value)
  {
    printf("\nPthread LIGHT LOG COND destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&main_thread1_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 1 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&main_thread1_cond);
  if(return_value)
  {
    printf("\nPthread main cond 1 destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&main_thread2_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 2 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&main_thread2_cond);
  if(return_value)
  {
    printf("\nPthread main cond 2 destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&main_thread3_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 3 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&main_thread3_cond);
  if(return_value)
  {
    printf("\nPthread main cond 3 destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&main_thread4_mutex);
  if(return_value)
  {
    printf("\nPthread main mutex 4 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&main_thread4_cond);
  if(return_value)
  {
    printf("\nPthread main cond 4 destroy: FAILED\n");
  }


  mq_close(light_log_mqdes1);
  mq_close(temp_log_mqdes1);
  mq_close(decision_mqdes1);

  printf("\nEXIT MAIN THREAD!! BYE BYE\n");
  return 0;
  
}
