#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
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
#define MAIN_QUEUE_NAME "/main_queue"
#define SOCKET_LOG_QUEUE_NAME "/socket_log_queue"
#define SOCKET_DECISION_QUEUE_NAME "/socket_decision_queue"
#define DECISION_QUEUE_NAME "/decision_queue"
#define FLAG (O_RDWR | O_CREAT)
#define DATA_SIZE 200

uint8_t exit_flag = 0;
uint32_t port_number = 0;

typedef enum pthread_Error
{ 
  PTHREAD_CREATE_FAIL = 1,
  PTHREAD_MUTEX_INIT_FAIL,
  PTHREAD_COND_INIT_FAIL,
  PTHREAD_MUTEX_DESTROY_FAIL,
  PTHREAD_COND_DESTROY_FAIl
}p_error;



/*enum for logger level*/
typedef enum logger_level
{
    SENSOR_DATA = 1,
    ERROR = 2,
    INFO = 3,
    ALERT = 4
}log_level;

/*enum for task id*/
typedef enum task_id
{ 
  main_tiva_task = 1,
  socket_tiva_task = 2,
  accelerometer_task = 3,
  alcohol_task = 4,
  CO_task = 5,
  ultrasonic_task = 6,
  main_bbg_task = 7,
  socket_task = 8,
  logger_task = 9,
  decision_task = 10
}tid;

/*message structure*/
typedef struct message_log
{
  char* current_time;  
  log_level logged_level;
  tid task_ID;
  char message_string[DATA_SIZE];
  char message[DATA_SIZE];
  uint8_t message_length;
}m_log;



typedef struct request_log
{
  tid Source_task_ID;
  tid Destination_task_ID;
  char command;
}r_log;

/*different structures for all tasks*/
m_log temp_tk;
m_log light_tk;
m_log maintask;
m_log decisiontask;

/*set up handler for control+C*/
sigset_t sigint_mask;
struct sigaction sigint_handler;
 

char final_file[MSG_SIZE];

/*mutex for different queues*/
pthread_mutex_t main_log_queue_mutex;
pthread_mutex_t decision_log_queue_mutex;
pthread_mutex_t socket_log_queue_mutex;
pthread_mutex_t socket_decision_queue_mutex;

/*mutex and condition variables for main to monitor four tasks*/
pthread_mutex_t main_thread1_mutex;
pthread_cond_t main_thread1_cond;

pthread_mutex_t main_thread2_mutex;
pthread_cond_t main_thread2_cond;

pthread_mutex_t main_thread3_mutex;
pthread_cond_t main_thread3_cond;

/*message queue*/
struct mq_attr attr;
mqd_t socket_log_mqdes1, socket_decision_mqdes1, main_log_mqdes1, decision_log_mqdes1;

/*Timer setup for Heartbeat*/
struct timespec heartbeat_setup(uint32_t second_value, uint32_t nanosec_value)
{
  struct timespec ts;
  struct timeval tp;

  gettimeofday(&tp,NULL);
  ts.tv_sec = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += second_value;
  ts.tv_nsec += nanosec_value;
  
  return ts;

}

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

/*Signal handler for SIGINT*/
void signal_handler()
{
  time_t a = time(NULL);
  maintask.current_time = ctime(&a);
  maintask.logged_level = INFO;
  maintask.task_ID = main_bbg_task;
  strcpy(maintask.message_string,"EXIT GRACEFULLYYY!!!!!!!!!!!");
  strcpy(maintask.message, " - ");
  maintask.message_length = strlen(maintask.message);       
  pthread_mutex_lock(&main_log_queue_mutex);
   
  /*send the message to the queue and check for success*/
  if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
  {
    printf("\nERROR: mqsend\n");
  }

  pthread_mutex_unlock(&main_log_queue_mutex);

  exit_flag = 1;
}

/*function to setup sigint*/
void setup_SIGINT()
{

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

}

/*function for socket thread*/
void *socket_function()
{
  int sockfd, newsockfd, client_length;

  struct sockaddr_in server_address, client_address;

  int n = 0;

  m_log receive;
  /*create a tcp socket*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0)
  { 
    printf("ERROR:SOCKET OPENING");
    exit(1);
  }

  bzero((char *)&server_address, sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_number);

  if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
  {
    printf("ERROR: BINDING");
  }

  listen(sockfd,5);
  client_length = sizeof(client_address);
  
  /*connection created*/
  newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_length);
  if(newsockfd < 0)
  {
    printf("\nERROR: accept\n");
  }

  while(1)
  {
    /*send condition to main indicating alive*/
    pthread_cond_broadcast(&main_thread1_cond);
    
    /*check for graceful exit*/
    if(exit_flag == 1)
    {
      break;
    }
    
    read(newsockfd,&receive,sizeof(receive));   
  
    /*lock the main queue mutex*/
    pthread_mutex_lock(&socket_log_queue_mutex);
   
    /*send the message to the queue and check for success*/
    if(mq_send(socket_log_mqdes1,(const char *)&receive, sizeof(receive),0) == -1)
    {
      printf("\nERROR: mqsend\n");
    }
   
    /*unlock the main queue mutex*/
    pthread_mutex_unlock(&socket_log_queue_mutex);
  
    if(receive.logged_level == SENSOR_DATA)
    {

      /*lock the main queue mutex*/
      pthread_mutex_lock(&socket_decision_queue_mutex);
   
      /*send the message to the queue and check for success*/
      if(mq_send(socket_decision_mqdes1,(const char *)&receive, sizeof(receive),0) == -1)
      {
        printf("\nERROR: mqsend\n");
      }
   
      /*unlock the main queue mutex*/
      pthread_mutex_unlock(&socket_decision_queue_mutex);
    }

    write(newsockfd,"yes",sizeof("yes"));
    sleep(150000);
  }

  close(newsockfd);
  pthread_exit(NULL);
}

/*function for decision thread*/
void *decision_function()
{
  while(1)
  {
 
    /*send condition signal to main indicating alive*/
    pthread_cond_broadcast(&main_thread2_cond); 

    /*check for graceful exit*/
    if(exit_flag == 1)
    {
        break;
    }

    char buffer[MSG_SIZE];
    
    /*lock the mutex and wait for timer to fire*/
    pthread_mutex_lock(&socket_decision_queue_mutex);
    
    printf("\nInside thread decision\n");  
    
    /*get the attributes for the queue*/
    mq_getattr(socket_decision_mqdes1,&attr);
    
    /*check if there is any messages in the queue*/
    if(attr.mq_curmsgs != 0)
    {
      static int Msg_no;
      char logger_level_string[200];
      char task_name[200];
      m_log receiver;
     
      /*read messages until queue is empty*/
      while(attr.mq_curmsgs != 0)
      {
        Msg_no++;
      
        /*get the message from queue*/
        int n = mq_receive(socket_decision_mqdes1,(char*)&receiver,MSG_SIZE,NULL);
        /*check for which task*/
        if(receiver.task_ID == accelerometer_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            decisiontask.current_time = ctime(&a);
            decisiontask.task_ID = decision_task;
            decisiontask.logged_level = ALERT;
            strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! ACCELEROMETER OUT OF CONTROL");
            sprintf(decisiontask.message,"%f",12.20);
            decisiontask.message_length = strlen(decisiontask.message);  
           
            /*lock the main queue mutex*/
            pthread_mutex_lock(&decision_log_queue_mutex);
    
            /*send the message to the queue and check for success*/
            if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
            {
              printf("\nERROR: mqsend\n");
              exit(1);
            }
              
            /*unlock the main queue mutex*/
            pthread_mutex_unlock(&decision_log_queue_mutex);
         }
        }
        else if(receiver.task_ID == alcohol_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            decisiontask.current_time = ctime(&a);
            decisiontask.task_ID = decision_task;
            decisiontask.logged_level = ALERT;
            strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! ALCOGOL DETECTED. HE IS DRUNKK!!!!");
            sprintf(decisiontask.message,"%f",2.0);
            decisiontask.message_length = strlen(decisiontask.message);  
          
            /*lock the main queue mutex*/
            pthread_mutex_lock(&decision_log_queue_mutex);
    
            /*send the message to the queue and check for success*/
            if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
            {
              printf("\nERROR: mqsend\n");
              exit(1);
            }
              
            /*unlock the main queue mutex*/
            pthread_mutex_unlock(&decision_log_queue_mutex);
         }
                 
        }
        else if(receiver.task_ID == CO_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            decisiontask.current_time = ctime(&a);
            decisiontask.task_ID = decision_task;
            decisiontask.logged_level = ALERT;
            strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! SMOKE DETECTED. FIRE FIRE FIRE!!!!");
            sprintf(decisiontask.message,"%f",20.0);
            decisiontask.message_length = strlen(decisiontask.message);  
          
          
            /*lock the main queue mutex*/
            pthread_mutex_lock(&decision_log_queue_mutex);
    
            /*send the message to the queue and check for success*/
            if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
            {
              printf("\nERROR: mqsend\n");
              exit(1);
            }
              
            /*unlock the main queue mutex*/
            pthread_mutex_unlock(&decision_log_queue_mutex);
         }
 
        }
        else if(receiver.task_ID == ultrasonic_task)
        {
          /*check if logged level is SENSOR DATA*/
          if(receiver.logged_level == 1)
          {
            time_t a = time(NULL);
            decisiontask.current_time = ctime(&a);
            decisiontask.task_ID = decision_task;
            decisiontask.logged_level = ALERT;
            strcpy(decisiontask.message_string,"ALERT!!! ALERT!!! ALERT!!! COLLISION MAY TAKE PLACE.. STOP STOP!!!!");
            sprintf(decisiontask.message,"%f",22.2);
            decisiontask.message_length = strlen(decisiontask.message);  
          
            /*lock the main queue mutex*/
            pthread_mutex_lock(&decision_log_queue_mutex);
    
            /*send the message to the queue and check for success*/
            if(mq_send(decision_log_mqdes1,(const char *)&decisiontask, sizeof(decisiontask),0) == -1)
            {
              printf("\nERROR: mqsend\n");
              exit(1);
            }
             
            /*unlock the main queue mutex*/
            pthread_mutex_unlock(&decision_log_queue_mutex);
          }
        }        
      }
 
      /*get attributes to check for remaining messages on queue*/
      mq_getattr(socket_decision_mqdes1,&attr);
    }
    
    /*unlock the mutex and exit*/
    pthread_mutex_unlock(&socket_decision_queue_mutex);
    usleep(150000);
  }
  
  /*exit the thread*/
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 4\n");
  pthread_exit(NULL);


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
    
    /*sleep*/
    usleep(15000);
  
  }
  
  printf("\nEXITTTTTTTTTTTTT THREADDDDDDDDDDDDDD 3\n");
  pthread_exit(NULL);

}




int main(int argc, char* argv[])
{
  char buffer[MSG_SIZE];
  
  /*set default file name and path to file*/
  char file_name[100] = "log.txt";
  char path_name[100];
  char *path = getenv("PWD");
  
  strcpy(path_name,path); 
  
  port_number = atoi(argv[1]);

  if(argc < 2)
  {
    printf("\nERROR:NO ARGUMENTS PASSED.\n");
    exit(1);

  }
  else if(argc == 4)
  {
    strcpy(file_name,argv[2]);
    strcpy(path_name, argv[3]);
  }
  else if(argc == 3)
  {
    strcpy(file_name,argv[2]);
  }
  else if(argc > 4)
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
    maintask.current_time = ctime(&a);
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
    maintask.current_time = ctime(&a);
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
    maintask.current_time = ctime(&a);
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
    ts = heartbeat_setup(2,4000000);
        
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
        printf("\nThread SOCKET is working\n");
      }
      else
      {
        /*if thread 1 is stuck, log it and call signal handler for graceful exit*/
        printf("\nSOCKET THREAD STUCK!!!!!!!!!!!\n");
        time_t a = time(NULL);
        maintask.current_time = ctime(&a);
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
        printf("\nThread DECISION is working\n");
      }
      else
      {

        /*if thread 2 is stuck, log it and call signal handler for graceful exit*/
        printf("\nTHREAD DECISION STUCK!!!!!\n");
        time_t a = time(NULL);
        maintask.current_time = ctime(&a);
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
          printf("\nThread logger is working\n");;
      }      
      else
      {
        /*if thread 3 is stuck, log it and call signal handler for graceful exit*/
        printf("\nTHREAD Logger STUCK!!!!!!!!\n");
        time_t a = time(NULL);

        maintask.current_time = ctime(&a);
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
