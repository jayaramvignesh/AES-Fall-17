#include "sig.h"

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
  pthread_cond_broadcast(&thread4_cond);
}

void setup_SIGUSR1()
{
  /*set up signal mask to block all in main thread*/
  sigfillset(&sigmask);

  /*Set up signal handlers for SIGUSR1*/
  action.sa_flags = 0;
  action.sa_handler = sigusr1_handler;
  if((sigaction(SIGUSR1, &action, (struct sigaction *)0)) == -1)
  {
    printf("\nERROR IN ADDING SIGINT HANDLER: SIGACTION FAILED\n");
  }
}

void setup_SIGUSR2()
{
  /*set up signal mask to block all in main thread*/
  sigfillset(&sigmask);

  /*Set up signal handlers for SIGUSR1*/
  action.sa_flags = 0;
 
  action.sa_handler = sigusr2_handler;
  if((sigaction(SIGUSR2, &action, (struct sigaction *)0)))
  {
    printf("\nERROR IN ADDING SIGINT HANDLER: SIGACTION FAILED\n");
  }

}

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
