#define _GNU_SOURCE

#include "main.h"
#include "message.h"
#include "timer.h"
#include "sig.h"
#include "temperature.h"
#include "light.h"
#include "logger.h"
#include "decision.h"

/*variable used to schedule threads*/
 sig_atomic_t count = 0;
 sig_atomic_t exit_flag = 0;
 sig_atomic_t main_count = 0;
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
  mq_unlink(QUEUE_NAME1);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUE_NAME1, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*removes the queue name if already existing*/
  mq_unlink(QUEUE_NAME2);

  /*create a queue with mqdes2 as the queue descriptor*/
  mqdes2 = mq_open(QUEUE_NAME2, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes2 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }


  /*get the attributes for the queue*/
  mq_getattr(mqdes1,&attr);

  printf("\n%ld MQDES1 messages  are currently on the queue\n",attr.mq_curmsgs);
  printf("\nMQDES1 size of queue %ld\n",attr.mq_maxmsg);
  
  /*get the attributes for the queue*/
  mq_getattr(mqdes2,&attr);

  printf("\n%ld MQDES2 messages  are currently on the queue\n",attr.mq_curmsgs);
  printf("\nMQDES2 size of queue %ld\n",attr.mq_maxmsg);
  
  setup_SIGINT();
  setup_SIGUSR1();
  setup_SIGUSR2();
  
  /*structure for timer*/
  struct itimerval timer;
  timer = setup_timer(0,250010);
    
  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&main_thread1_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
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

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&thread1_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&thread1_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }

  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&thread2_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&thread2_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }


  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&thread3_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&thread3_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }
 
  /*initialize mutex and check for return*/
  if(pthread_mutex_init(&thread4_mutex,NULL)!=0)
  {
      printf("\nERROR: MUTEX INIT FAILED \n");
      return -1;
  }

  /*initialize condition variable and check for return*/
  if(pthread_cond_init(&thread4_cond,NULL)!=0)
  {
      printf("\nERROR: CONDITION INIT FAILED \n");
      return -1;
  }


  /*create a thread*/
  pthread_t thread1,thread2,thread3,thread4;

  int return_value = pthread_create(&thread1,NULL,&temperature_function,NULL);
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
    strcpy(maintask.message, "TEMPERATURE TASK SPAWNED!!!!!!");
    maintask.message_length = strlen(maintask.message); 
   
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }  
  }

  /*create a thread*/
  return_value = pthread_create(&thread2,NULL,&light_function,NULL);
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
    strcpy(maintask.message, "LIGHT TASK SPAWNED!!!!!!");
    maintask.message_length = strlen(maintask.message); 
   
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }  
  }

 
  /*create a thread*/
  return_value = pthread_create(&thread3,NULL,&logger_function,NULL);
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
    strcpy(maintask.message, "LOGGER TASK SPAWNED!!!!!!");
    maintask.message_length = strlen(maintask.message); 
   
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }  
  }

  return_value = pthread_create(&thread4,NULL,&decision_function,NULL);
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
    strcpy(maintask.message, "DECISION TASK SPAWNED!!!!!!");
    maintask.message_length = strlen(maintask.message); 
   
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }  
  }


  /*set the timer*/
  setitimer(ITIMER_REAL, &timer, NULL);

  struct timespec ts;
  
  while(1)
  {

    ts = heartbeat_setup(2,4000000);
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
        task1.current_time = ctime(&a);
        task1.logged_level = ERROR;
        task1.task_ID = light_task;
        strcpy(task1.message, "TEMPERAETURE THREAD STUCK!!!!!! ABORTING EVERYTHING");
        task1.message_length = strlen(task1.message); 
      
        /*send the message to the queue and check for success*/
        if(mq_send(mqdes1,(const char *)&task1, sizeof(task1),0) == -1)
        {
          printf("\nERROR: mqsend\n");
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
        task2.current_time = ctime(&a);
        task2.logged_level = ERROR;
        task2.task_ID = light_task;
        strcpy(task2.message, "THREAD LIGHT STUCK!!!! ABORTYING EVERYTHING");
        task2.message_length = strlen(task2.message); 
    
        /*send the message to the queue and check for success*/
        if(mq_send(mqdes1,(const char *)&task2, sizeof(task2),0) == -1)
        {
          printf("\nERROR: mqsend\n");
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
        task3.current_time = ctime(&a);
        task3.logged_level = ERROR;
        task3.task_ID = light_task;
        strcpy(task3.message, "LOGGER TASK STUCK!!!! ABORTYING EVERYTHING");
        task3.message_length = strlen(task3.message); 
    
        /*send the message to the queue and check for success*/
        if(mq_send(mqdes1,(const char *)&task3, sizeof(task3),0) == -1)
        {
          printf("\nERROR: mqsend\n");
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
        printf("\nTHREAD 4 FASSS GAYA\n");
        time_t a = time(NULL);
        task1.current_time = ctime(&a);
        task1.logged_level = ERROR;
        task1.task_ID = decision_task;
        strcpy(task1.message, "DECISION THREAD STUCK!!!!!! ABORTING EVERYTHING");
        task1.message_length = strlen(task1.message); 
      
        /*send the message to the queue and check for success*/
        if(mq_send(mqdes1,(const char *)&task1, sizeof(task1),0) == -1)
        {
          printf("\nERROR: mqsend\n");
        }
        signal_handler();
        continue;
      }

      printf("\nMAINNNNNNNNN COUNTTTTTTTT %d\n",main_count);
      if(main_count == 45)
      {
        printf("\nIDHAR HUN MAIN\n");
        main_count = 0;
        pthread_kill(thread1,SIGUSR1) ;
      }
    }
  }

  /*wait for thread to terminate*/
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_join(thread3,NULL); 
  pthread_join(thread4,NULL);

  printf("\nAll children threads are dead\n");
   
  /*close the queue*/
  mq_close(mqdes1);
  
  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&thread1_mutex);
  if(return_value)
  {
    printf("\nPthread mutex 1 destroy: FAILED\n");
  }
  
  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&thread1_cond);
  if(return_value)
  {
    printf("\nPthread cond 1 destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&thread2_mutex);
  if(return_value)
  {
    printf("\nPthread mutex 2 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&thread2_cond);
  if(return_value)
  {
    printf("\nPthread cond 2 destroy: FAILED\n");
  }
  
  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&thread3_mutex);
  if(return_value)
  {
    printf("\nPthread mutex 3 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&thread3_cond);
  if(return_value)
  {
    printf("\nPthread cond 3 destroy: FAILED\n");
  }

  /*destroy the mutex*/
  return_value = pthread_mutex_destroy(&thread4_mutex);
  if(return_value)
  {
    printf("\nPthread mutex 1 destroy: FAILED\n");
  }
  
  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&thread4_cond);
  if(return_value)
  {
    printf("\nPthread cond 1 destroy: FAILED\n");
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
    printf("\nPthread main mutex 1 destroy: FAILED\n");
  }

  /*destroy the condition variable*/
  return_value =  pthread_cond_destroy(&main_thread4_cond);
  if(return_value)
  {
    printf("\nPthread main cond 1 destroy: FAILED\n");
  }


  printf("\nEXIT MAIN.. BYE BYE!!!!\n");
  pthread_exit(NULL);
}
