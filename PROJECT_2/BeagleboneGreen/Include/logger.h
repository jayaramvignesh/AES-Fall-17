/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: logger.h
*
*   function prototypes
*   - logger_function() function for logger thread
*   - logger_id_decode() function to decode the log level for the packets
*   - logger_task_name() function to decode the task id for the packets
*
*****************************************************************************/



#ifndef _LOGGER_H
#define _LOGGER_H

#include "main.h"
#include "sig.h"
#include "timer.h"

void *logger_function();
void logger_id_decode(char logger_level_string[], m_log receiver);
void logger_task_name(char task_name[], m_log receiver);

#endif
