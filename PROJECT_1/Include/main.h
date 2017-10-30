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
#include<errno.h>
#include<sys/time.h>


/*vatraible used to schedule threads*/
extern sig_atomic_t count;
extern sig_atomic_t exit_flag;
extern sig_atomic_t main_count;

 /*final path and file name stored*/
char final_file[100];

/*mutex and condition variables for three tasks*/
pthread_mutex_t thread1_mutex;
pthread_cond_t thread1_cond;

pthread_mutex_t thread2_mutex;
pthread_cond_t thread2_cond;


pthread_mutex_t thread3_mutex;
pthread_cond_t thread3_cond;

pthread_mutex_t thread4_mutex;
pthread_cond_t thread4_cond;


/*mutex and condition variables for main to monitor three tasks*/
pthread_mutex_t main_thread1_mutex;
pthread_cond_t main_thread1_cond;

pthread_mutex_t main_thread2_mutex;
pthread_cond_t main_thread2_cond;

pthread_mutex_t main_thread3_mutex;
pthread_cond_t main_thread3_cond;

pthread_mutex_t main_thread4_mutex;
pthread_cond_t main_thread4_cond;


#endif


