/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 2nd Nov 2017
*
*   file: sig.c
*
*   description:
*
*   -source file for signal
*
*   - signal handler definition
*     - on receiving signal, broadcast condition, add to log and set exit flag
*
*   - setup the signal
*       
*****************************************************************************/




#include "sig.h"

/*Signal handler for SIGINT*/
void signal_handler()
{
  time_t a = time(NULL);
  maintask.current_time = ctime(&a);
  maintask.logged_level = INFO;
  maintask.task_ID = main_task;
  strcpy(maintask.message_string,"EXIT GRACEFULLYYY!!!!!!!!!!!");
  strcpy(maintask.message, " - ");
  maintask.message_length = strlen(maintask.message); 
   
  pthread_cond_broadcast(&temp_task_cond);
  pthread_cond_broadcast(&light_task_cond);
 
  usleep(10000);
  /*send the message to the queue and check for success*/
  if(mq_send(main_log_mqdes1,(const char *)&maintask, sizeof(maintask),0) == -1)
  {
    printf("\nERROR: mqsend\n");
  }
  else if(main_log_queue_count < 10)
  {
    main_log_queue_count++;
    printf("\nIN SIGINT HANDLER, log_queue_count %d\n",main_log_queue_count);
  }  
  
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


