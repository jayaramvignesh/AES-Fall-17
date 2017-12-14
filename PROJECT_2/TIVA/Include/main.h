/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: main.h
*
*   description:
*
*   -Main header file for project2
*
*****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U


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
  uart_tiva_task = 2,
  accelerometer_task = 3,
  alcohol_task = 4,
  CO_task = 5,
  ultrasonic_task = 6,
  main_bbg_task = 7,
  uart_task = 8,
  logger_task = 9,
  decision_task = 10
}tid;

/*packet structure for sensor data*/
typedef struct packet
{
    uint8_t log_id;
    uint8_t log_level;
    float data;
    char timestamp[25];
    char c;
}send_pkt;
#endif /* MAIN_H_ */
