#ifndef _SIG_H
#define _SIG_H

#include "main.h"
#include "message.h"
#include "timer.h"

sigset_t sigmask;
struct sigaction action;

/*set up handler for control+C*/
sigset_t sigint_mask;
struct sigaction sigint_handler;
 

void setup_SIGUSR1();
void setup_SIGUSR2();
void setup_SIGINT();

#endif


