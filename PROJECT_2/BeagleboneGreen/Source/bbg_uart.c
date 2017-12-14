/****************************************************************************
*   Authors: Arundhath Swami and Vignesh Jayaram
*   date edited: 13th Dec  2017
*
*   file: bbg_uart.c
*
*   description:
*
*   -source file for setup of uart
*
*   -setup for uart is performed
*
*       
*****************************************************************************/




#include "bbg_uart.h"

#define FLAGS O_RDWR | O_NOCTTY | O_SYNC
char *device_name = "/dev/ttyO4";

void tty_config(struct termios *config, int file_descriptor)
{
  tcgetattr(file_descriptor, config);
  config->c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
  config->c_oflag = 0;
  config->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
  config->c_cc[VMIN] = 1;
  config->c_cc[VTIME] = 0;
  if(cfsetispeed(config, B57600) || cfsetospeed(config, B57600))
  {
    perror("ERROR in baud set\n");
  }


  if(tcsetattr(file_descriptor, TCSAFLUSH, config) < 0)
  {
    perror("ERROR in set attr\n");
  }                                                 
}

void uart_init(void)
{
   
  fd = open(device_name,FLAGS );
  
  if(fd == -1)
  {
    perror("ERROR opening file descriptor\n");  
  }
  
  configure = (struct termios*)malloc(sizeof(struct termios));
  tty_config(configure, fd);

  if(tcsetattr(fd,TCSAFLUSH, configure) < 0)
  {
    printf("\nERROR: TC Set Attr\n");
  }
}


