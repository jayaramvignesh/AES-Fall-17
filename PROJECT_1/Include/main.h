/*************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
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

#define SEC_VALUE 0
#define USEC_VALUE 100000

/*final file name with path to be stored*/
char final_file[100];

typedef enum pthread_Error
{ 
  PTHREAD_CREATE_FAIL = 1,
  PTHREAD_MUTEX_INIT_FAIL,
  PTHREAD_COND_INIT_FAIL,
  PTHREAD_MUTEX_DESTROY_FAIL,
  PTHREAD_COND_DESTROY_FAIl
}p_error;


/*mutex for different queues*/
pthread_mutex_t main_log_queue_mutex;
pthread_mutex_t temp_log_queue_mutex;
pthread_mutex_t light_log_queue_mutex;
pthread_mutex_t decision_queue_mutex;

/*condition variables using which the tasks will collect data periodically*/
pthread_cond_t temp_task_cond;
pthread_cond_t light_task_cond;

/*mutex and condition variables for main to monitor four tasks*/
pthread_mutex_t main_thread1_mutex;
pthread_cond_t main_thread1_cond;

pthread_mutex_t main_thread2_mutex;
pthread_cond_t main_thread2_cond;

pthread_mutex_t main_thread3_mutex;
pthread_cond_t main_thread3_cond;

pthread_mutex_t main_thread4_mutex;
pthread_cond_t main_thread4_cond;

/*global variables*/
sig_atomic_t main_log_queue_count;
sig_atomic_t light_log_queue_count;
sig_atomic_t temp_log_queue_count; 
sig_atomic_t decision_queue_count;
sig_atomic_t exit_flag ;
sig_atomic_t main_count;

extern sig_atomic_t temp_task_period ;
extern sig_atomic_t light_task_period;


#endif

