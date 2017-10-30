#define _GNU_SOURCE

#include <unistd.h>
#include<mqueue.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdbool.h>
#include<errno.h>
#include<pthread.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<time.h>
#include<errno.h>
#include<sys/time.h>

#define MSG_SIZE 4096
#define NO_OF_MGS 10
#define QUEUE_NAME "/queue"
#define FLAG (O_RDWR | O_CREAT)
#define DATA_SIZE 200

/*enum for logger level*/
typedef enum logger_level
{
    SENSOR_DATA = 1,
    ERROR = 2,
    INFO = 3
}log_level;

/*enum for task id*/
typedef enum task_id
{
    main_task = 1,
    temperature_task = 2,
    light_task = 3,
    logger_task = 4
}tid;

/*message structure*/
typedef struct message_log
{
  char* current_time;  
  log_level logged_level;
  tid task_ID;
  char message[DATA_SIZE];
  uint8_t message_length;
}m_log;
  
/*message queue*/
struct mq_attr attr;
mqd_t mqdes1;

/*final path and file name stored*/
char final_file[100];

/*mutex and condition variables for three tasks*/
pthread_mutex_t thread1_mutex;
pthread_cond_t thread1_cond;

pthread_mutex_t thread2_mutex;
pthread_cond_t thread2_cond;


pthread_mutex_t thread3_mutex;
pthread_cond_t thread3_cond;

/*mutex and condition variables for main to monitor three tasks*/
pthread_mutex_t main_thread1_mutex;
pthread_cond_t main_thread1_cond;

pthread_mutex_t main_thread2_mutex;
pthread_cond_t main_thread2_cond;

pthread_mutex_t main_thread3_mutex;
pthread_cond_t main_thread3_cond;

/*different structures for all tasks*/
m_log task1;
m_log task2;
m_log task3;
m_log maintask;

/*vatraible used to schedule threads*/
sig_atomic_t count = 0;
sig_atomic_t exit_flag = 0;
sig_atomic_t main_count = 0;

/*sigusr1 handler*/
void sigusr1_handler()
{
  printf("\nIDHAR HUN JHANDLERRRRRRRRRRRR MAIN\n");
  sleep(2);
}

/*sigusr1 handler*/
void sigusr2_handler()
{
  printf("\nIDHAR HUN JHANDLERRRRRRRRRRRR MAIN\n");
  pthread_cond_broadcast(&thread2_cond);
}


/*Signal handler for SIGINT*/
void signal_handler()
{
  exit_flag = 1;
  count = 10;
  time_t a = time(NULL);
  maintask.current_time = ctime(&a);
  maintask.logged_level = INFO;
  maintask.task_ID = main_task;
  strcpy(maintask.message, "EXIT GRACEFULLY!!!!!!");
  maintask.message_length = strlen(maintask.message); 
   
  /*send the message to the queue and check for success*/
  if(mq_send(mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
  {
    printf("\nERROR: mqsend\n");
  }  
  
  pthread_cond_broadcast(&thread1_cond);
  pthread_cond_broadcast(&thread2_cond);
  pthread_cond_broadcast(&thread3_cond);
}

/*handler for timer*/
void timer_handler(int signum)
{
  main_count++;
  count++;
  printf("\ntimer expired %d times\n",count);
  if(count == 18)
  {
    exit(1);
  }
  if(exit_flag == 1)
  {
    pthread_cond_broadcast(&thread1_cond);
    pthread_cond_broadcast(&thread2_cond);
    pthread_cond_broadcast(&thread3_cond);
  }
  if(count == 1)
  {
    pthread_cond_broadcast(&thread1_cond);
  }
  else if(count == 2)
  {
    pthread_cond_broadcast(&thread2_cond);
  }
  else if(count == 3)
  {
    pthread_cond_broadcast(&thread3_cond);
  }
}

/*function for thread1*/
void *function1()
{
  while(1)
  {

    pthread_cond_broadcast(&main_thread1_cond);
    time_t a = time(NULL);
    task1.current_time = ctime(&a);
    task1.logged_level = SENSOR_DATA;
    task1.task_ID = temperature_task;
    int data = 25;
    sprintf(task1.message,"%d",data);
    task1.message_length = strlen(task1.message);
  
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread1_mutex);
    
    pthread_cond_wait(&thread1_cond,&thread1_mutex);  
    if(exit_flag == 1)
    {
      break;
    }

    printf("\nInside thread 1\n");  
    
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&task1, sizeof(task1),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }

   /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread1_mutex);
    printf("\nmessage sent\n");  
  }

  pthread_mutex_unlock(&thread1_mutex);
  printf("\nEXITTTTTTTTTTTT THREADDDDDDDDDDDDDD 1\n");
  pthread_exit(NULL);
}

/*function for thread2*/
void *function2()
{
  while(1)
  {

    pthread_cond_broadcast(&main_thread2_cond);
    time_t a = time(NULL);
    task2.current_time = ctime(&a);
    task2.logged_level = SENSOR_DATA;
    task2.task_ID = light_task;
    int data = 100;
    sprintf(task2.message,"%d",data);
    task2.message_length = strlen(task2.message);
    
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&thread2_mutex);
    pthread_cond_wait(&thread2_cond,&thread2_mutex); 
    if(exit_flag == 1)
    {
      break;
    }
    printf("\nInside thread 2\n");  

   /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&task2, sizeof(task2),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
 
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&thread2_mutex);
  
    printf("\nmessage sent\n");  
  }
  
  pthread_mutex_unlock(&thread2_mutex);
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDD 2\n");
  pthread_exit(NULL);
}

/*function for thread 3*/
void *function3()
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

int main(int argc, char* argv[])
{
  char buffer[MSG_SIZE];
  
  /*get file name and path to file*/
  char file_name[100] = "logger.txt";
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
  mq_unlink(QUEUE_NAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUE_NAME, FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(mqdes1,&attr);

  printf("\n%ld messages  are currently on the queue\n",attr.mq_curmsgs);
  printf("\nsize of queue %ld\n",attr.mq_maxmsg);
  
  /*set up handler for control+C*/
  sigset_t sigint_mask;
  struct sigaction sigint_handler;
  int set1 = 0;
  
  /*set up signal mask to block all in main thread*/
  sigfillset(&sigint_mask);


  /*Set up signal handlers for SIGINT*/
  sigint_handler.sa_flags = 0;
  sigint_handler.sa_handler = signal_handler;
  if((sigaction(SIGINT, &sigint_handler, (struct sigaction *)0)) == -1)
  {
    printf("\nERROR IN ADDING SIGINT HANDLER: SIGACTION FAILED\n");
  }
  else
  {
    printf("\nSIGINT HANDLER INSTALLATION SUCCESSFULL\n");
  }


  /*structure for timer*/
  struct sigaction sa;
  struct itimerval timer;

  /*clear out the sigaction instance*/
  memset(&sa,0,sizeof(sa));

  /*set the handler*/
  sa.sa_handler = &timer_handler;
  sigaction(SIGALRM, &sa, NULL);

  /*set a periodic timer for 500 ms*/
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 250010;

  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 250010;
  
  sigset_t sigmask;
  struct sigaction action; 

  /*set up signal mask to block all in main thread*/
  sigfillset(&sigmask);

  /*Set up signal handlers for SIGUSR1*/
  action.sa_flags = 0;
  action.sa_handler = sigusr1_handler;
  if((sigaction(SIGUSR1, &action, (struct sigaction *)0)) == -1)
  {
    printf("\nERROR IN ADDING SIGINT HANDLER: SIGACTION FAILED\n");
  }

  action.sa_handler = sigusr2_handler;
  if((sigaction(SIGUSR2, &action, (struct sigaction *)0)))
  {
    printf("\nERROR IN ADDING SIGINT HANDLER: SIGACTION FAILED\n");
  }


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
 
  /*create a thread*/
  pthread_t thread1,thread2,thread3;
  int return_value = pthread_create(&thread1,NULL,&function1,NULL);
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
  return_value = pthread_create(&thread2,NULL,&function2,NULL);
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
  return_value = pthread_create(&thread3,NULL,&function3,NULL);
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


  /*set the timer*/
  setitimer(ITIMER_REAL, &timer, NULL);

  struct timespec ts;
  struct timeval tp;

 
  
  while(1)
  {
    gettimeofday(&tp,NULL);
    ts.tv_sec = tp.tv_sec;
    ts.tv_nsec = tp.tv_usec * 1000;
    ts.tv_sec += 1;
    ts.tv_nsec += 4000000;
    
    if(exit_flag == 1)
    {
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

      printf("\nMAINNNNNNNNN COUNTTTTTTTT %d\n",main_count);
      if(main_count == 42)
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


  printf("\nEXIT MAIN.. BYE BYE!!!!\n");
  pthread_exit(NULL);
}
