/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 14th Dec 2017
*
*   file: main.h
*
*   description:
*
*   -Main header file for project1
*
*****************************************************************************/




#ifndef _MAIN_H
#define _MAIN_H


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
#include<errno.h>
#include<pthread.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<time.h>
#include<errno.h>
#include<sys/time.h>
#include <termios.h>
#include <sys/stat.h>

#define MSG_SIZE 4096
#define NO_OF_MGS 10
#define MAIN_QUEUE_NAME "/main_queue"
#define SOCKET_LOG_QUEUE_NAME "/socket_log_queue"
#define SOCKET_DECISION_QUEUE_NAME "/socket_decision_queue"
#define DECISION_QUEUE_NAME "/decision_queue"
#define FLAG (O_RDWR | O_CREAT)
#define DATA_SIZE 200


uint8_t exit_flag;
uint32_t port_number;

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
  char current_time[26];  
  log_level logged_level;
  tid task_ID;
  char message_string[DATA_SIZE];
  char message[DATA_SIZE];
  uint8_t message_length;
}m_log;


typedef struct receive_uart_packet
{
  uint8_t log_id;
  uint8_t log_level;
  float data;
  char timestamp[25];
  char c;
}uart_packet;



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


#endif
