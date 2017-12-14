/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 14th Dec 2017
*
*   file: uart_task.h
*
*   function prototypes
*   - socket_function() function for uart thread
*   - log_id_decode() functoin to decode the log id for packet formation 
*   - log_task_name() function to decode the task name for packet formation
*
**************************************************************************/


#ifndef _UART_TASK_H
#define _UART_TASK_H

#include "main.h"
#include "sig.h"

void *socket_function();
void log_id_decode(char logger_level_string[], uart_packet receiver);
void log_task_name(char task_name[], uart_packet receiver);


#endif
