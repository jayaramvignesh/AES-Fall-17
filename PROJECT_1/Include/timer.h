#ifndef _TIMER_H
#define _TIMER_H

#include "main.h"
#include "message.h"

  void signal_handler();

 struct itimerval  setup_timer( uint32_t sec_value, uint32_t usec_value);

  struct timespec heartbeat_setup(uint32_t second_value, uint32_t nanosec_value);

#endif
