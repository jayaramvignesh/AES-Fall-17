/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 6th Dec  2017
*
*   file: socket_bbg.c
*
*   description:
*
*   -source file for socket thread
*
*   -this thread sends a signal to release the main thread cond1
*   and to indicate it is alive. This is done at start of while 1
*
*   -exit flag is checked for pthread_exit
*
*   -this tasks receives data from tiva over TCP socket and adds them to the 
*    logger and decision queues
*       
*****************************************************************************/



#include "socket_bbg.h"

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


