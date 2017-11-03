/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: message.h
*
*   description:
*
*   -header file containing global declarations for queue.
*
*****************************************************************************/





#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "main.h"

#define MSG_SIZE 4096
#define NO_OF_MGS 10
#define MAIN_LOG_QUEUE_NAME "/main_log_queue"
#define TEMP_LOG_QUEUE_NAME "/temp_log_queue"
#define LIGHT_LOG_QUEUE_NAME "/light_log_queue"
#define FLAG (O_RDWR | O_CREAT)
#define DECISION_QUEUE_NAME "/decision_queue"
#define DATA_SIZE 200
#define TEMP_REQ_QUEUE_NAME "/temp_req_queue"
#define LIGHT_REQ_QUEUE_NAME "/light_req_queue"

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
    logger_task = 4,
    decision_task = 5
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

/*structure for request*/
typedef struct request_log
{
  tid Source_task_ID;
  tid Destination_task_ID;
  char command;
  uint32_t delay;
}r_log;

/*different structures for all tasks*/
m_log temp_tk;
m_log light_tk;
m_log maintask;

/*message queue*/
struct mq_attr attr;
mqd_t temp_log_mqdes1, light_log_mqdes1, decision_mqdes1, main_log_mqdes1;
mqd_t tr_mqdes1 , lr_mqdes1 ;


#endif



