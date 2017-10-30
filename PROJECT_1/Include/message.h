#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "main.h"

#define MSG_SIZE 4096
#define NO_OF_MGS 10
#define QUEUE_NAME1 "/queue1"
#define QUEUE_NAME2 "/queue2"
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
    logger_task = 4,
    decision_task = 5
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
 
/*different structures for all tasks*/
m_log task1;
m_log task2;
m_log task3;
m_log maintask;

/*message queue*/
struct mq_attr attr;
mqd_t mqdes1;
mqd_t mqdes2;


#endif 
