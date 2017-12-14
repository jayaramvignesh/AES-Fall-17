#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdint.h>

#define FLAGS O_RDWR | O_NOCTTY | O_SYNC

char *device_name = "/dev/ttyO4";

typedef struct receive_uart_packet
{
  uint8_t log_id;
  uint8_t log_level;
  float data;
  char timestamp[25];
  char c;
}uart_packet;

int fd;


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
  struct termios *configure; 
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

int main()
{
  int n = 0;
  uart_packet rec;
  uart_init();

  printf("\n-----------------------------------------------\n");
  printf("\nUART TESTING FOR SENDING DATA FROM TIVA TO BBG\n");
  printf("\n-----------------------------------------------\n");
  
  while(1)
  {
    /*Waiting to receive the thread*/
    printf("\nWAITING TO RECEIVE THE DATA FROM TIVA UART\n");
    
    n=read(fd,&rec,sizeof(rec));
    
    /*check if data is received*/
    if(n <= 0)
    {
      printf("\nRead Fail\n");
    }
    else
    {
      printf("\nreceived timestamp is %s\n",rec.timestamp);
      printf("\nData is %f\n",rec.data);
    }
  
  }
 
}

