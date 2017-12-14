/****************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
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

char *device = "/dev/ttyO4";
void tty_config(struct termios *con, int descriptor)
{
  tcgetattr(descriptor, con);
  con->c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
  con->c_oflag = 0;
  con->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
  con->c_cc[VMIN] = 1;
  con->c_cc[VTIME] = 0;
  if(cfsetispeed(con, B57600) || cfsetospeed(con, B57600))
  {
    perror("ERROR in baud set\n");
  }


  if(tcsetattr(descriptor, TCSAFLUSH, con) < 0)
  {
    perror("ERROR in set attr\n");
  }                                                 
}

void uart_init(void)
{
   
  fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);// | O_NDELAY);
  
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

/*int main()
{
  uart_init();
  
 
  char read_value[200]; 
  memset(read_value,'\0',sizeof(read_value));
  printf("\nWaiting to receive\n");
  int n=0;


  while(1)
  {
    uart_packet rec;
    printf("\nREADING AGAIN\n");
    if((n=read(fd,&rec,sizeof(rec))) < 0)
    {
      printf("\nRead Fail\n");
    }
    printf("\nlog elvel %d\n",rec.log_level);
    printf("\nlog id %d\n",rec.log_id);
    printf("\nDATA %f\n",rec.data);
    printf("\n%s\n",rec.timestamp);
    memset(rec,'\0',sizeof(rec));
  }
  
  close(fd);
}*/
