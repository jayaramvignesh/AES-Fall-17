/***********************************************************************************
* Authors: Arundhathi and Vignesh
* Date Edited: Dec 6,2017
*
* filename: unit_test_mqueue.c
*
* This file is used to test the mqueue tests
*
* The file tests for four cases:
*
* case1: test for adding more messages to a full queue
* case2: test for adding messages to a non full queue
* case3: test for removing messages from empty queue
* case4: test for removing messages from non empty queue
*
*
* ******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

typedef enum queue_return
{
  FULL_QUEUE_ADD = 1,
  SUCCESS_ADD,
  SUCCESS_REMOVE,
  QUEUE_EMPTY_REMOVE

}return_queue;

#define SIZE_OF_MESSAGE 4096
#define MAX_ALLOWED_MSGS 2
#define QUEUENAME "/queue"
#define QUEUE_FLAG (O_RDWR | O_CREAT | O_NONBLOCK)

/*function to addyy to full message queue*/
return_queue mqueue_addfull_fail(void)
{
  /*message queue*/
  struct mq_attr attr;
  mqd_t mqdes1;

  /*setting attributes for the message queue*/
  attr.mq_maxmsg = MAX_ALLOWED_MSGS;
  attr.mq_msgsize = SIZE_OF_MESSAGE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(QUEUENAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUENAME, QUEUE_FLAG , 0644, &attr);
  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  
  char packet[] = " UNIT TEST";
  if(mq_send(mqdes1,(const char *)&packet, sizeof(packet),0) == -1)
  {
      return FULL_QUEUE_ADD;
  }
  
  char packet2[] = "HI";
  if(mq_send(mqdes1,(const char *)&packet2, sizeof(packet2),0) == -1)
  {
      return FULL_QUEUE_ADD;
  }
  
  char packet3[] = "HI";
  if(mq_send(mqdes1,(const char *)&packet3, sizeof(packet3),0) == -1)
  {
      return FULL_QUEUE_ADD;
 }

}


/*functin to add to a message queue which is not full*/
return_queue mqueue_successfull_adds(void)
{
  /*message queue*/
  struct mq_attr attr;
  mqd_t mqdes1;

  /*setting attributes for the message queue*/
  attr.mq_maxmsg = MAX_ALLOWED_MSGS;
  attr.mq_msgsize = SIZE_OF_MESSAGE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(QUEUENAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUENAME, QUEUE_FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(mqdes1,&attr);

   char packet[] = " UNIT TEST";
  /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&packet, sizeof(packet),0) == -1)
    {
      return FULL_QUEUE_ADD;
    }
 
    /*send the message to the queue and check for success*/
    if(mq_send(mqdes1,(const char *)&packet, sizeof(packet),0) == -1)
    {
      return FULL_QUEUE_ADD;
    }
    else
    {
      return SUCCESS_ADD;
    } 
}

/*functon to remove from empty queue*/
return_queue mqueue_emptyremove_fail(void)
{
  /*message queue*/
  struct mq_attr attr;
  mqd_t mqdes1;

  /*setting attributes for the message queue*/
  attr.mq_maxmsg = MAX_ALLOWED_MSGS;
  attr.mq_msgsize = SIZE_OF_MESSAGE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(QUEUENAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUENAME, QUEUE_FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(mqdes1,&attr);

  
  char *receive_msg;  
  int n = mq_receive(mqdes1,(char*)&receive_msg,SIZE_OF_MESSAGE,NULL);
  
  if(n<0)
  {
    return QUEUE_EMPTY_REMOVE;
  }
  else
  {
    return SUCCESS_REMOVE;
  }

   
}

/*function to remove from queue which is not empty*/
return_queue mqueue_successfull_remove(void)
{
  /*message queue*/
  struct mq_attr attr;
  mqd_t mqdes1;

  /*setting attributes for the message queue*/
  attr.mq_maxmsg = MAX_ALLOWED_MSGS;
  attr.mq_msgsize = SIZE_OF_MESSAGE;
  attr.mq_flags = 0;
  
  /*removes the queue name if already existing*/
  mq_unlink(QUEUENAME);

  /*create a queue with mqdes1 as the queue descriptor*/
  mqdes1 = mq_open(QUEUENAME, QUEUE_FLAG , 0644, &attr);

  /*check for successful mq_open*/
  if(mqdes1 == -1)  
  {
    printf("\nERROR mq_open\n");
    exit(1);
  }

  /*get the attributes for the queue*/
  mq_getattr(mqdes1,&attr);

  char packet[] = " UNIT TEST";
  /*send the message to the queue and check for success*/
  if(mq_send(mqdes1,(const char *)&packet, sizeof(packet),0) == -1)
  {
    printf("\nERROR: mqsend\n");
  }
  
  char *receive_msg;  
  int n = mq_receive(mqdes1,(char*)&receive_msg,SIZE_OF_MESSAGE,NULL);
  
  if(n<0)
  {
    return QUEUE_EMPTY_REMOVE;
  }
  else
  {
    return SUCCESS_REMOVE;
  }

   
}



int main()
{

	printf("\nUNIT TEST : MESSAGE QUEUE TESTING\n");
	printf("\n______________________________________________________\n");

  uint32_t res;

  /******TEST TO REMOVE FROM EMPTY QUEUE*******/
  res = mqueue_emptyremove_fail();
	if(res == QUEUE_EMPTY_REMOVE)
		printf("\nUNIT TEST 1:REMOVE FROM EMPTY QUEUE: SUCCESS\n");
	else
		printf("\nUNIT TEST 1: REMOVE FROM EMPTY QUEUE - FAILURE\n");


  /******TEST TO REMOVE FROM NON EMPTY QUEUE*******/
  res = mqueue_successfull_remove();
	if(res == SUCCESS_REMOVE)
		printf("\nUNIT TEST 2: REMOVE FROM NON EMPTY QUEUE: SUCCESS\n");
	else
		printf("\nUNIT TEST 2: REMOVE FROM NON EMPTY QUEUE - FAILURE\n");


  /******TEST TO ADD TO FULL QUEUE*******/
  res = mqueue_addfull_fail();
	if(res == FULL_QUEUE_ADD)
		printf("\nUNIT TEST 3: ADD TO FULL QUEUE: SUCCESS\n");
	else
		printf("\nUNIT TEST 3: ADD TO FULL QUEUE - FAILURE\n");
 
  /******TEST TO ADD TO QUEUE THAT IS NOT FULL*******/
  res = mqueue_successfull_adds();
	if(res == SUCCESS_ADD)
		printf("\nUNIT TEST 4: ADD TO NON FULL QUEUE: SUCCESS\n");
	else
		printf("\nUNIT TEST 4: ADD TO NON FULL QUEUE - FAILURE\n");
}
