#ifndef _SIG_H
#define _SIG_H

#include "main.h"
#include "message.h"
#include "timer.h"

/*set up handler for control+C*/
sigset_t sigint_mask;
struct sigaction sigint_handler;
 

void setup_SIGINT();

#endif


