#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdint.h>

#define FLAGS O_RDWR | O_NOCTTY | O_SYNC

int uart_init(void)
{
	int file_descriptor = 0;
	char *device = "/dev/ttyO4";
  if ((file_descriptor = open(device, FLAGS))<0)
  {
    printf("\nERROR: UART FILE OPEN FAILED\n");
    return -1;
	}

	struct termios config_option;
	tcgetattr(file_descriptor, &config_option);
	config_option.c_iflag &= ~(IGNBRK | BRKINT | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	config_option.c_oflag = 0;
	config_option.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);	
	cfsetispeed(&config_option, B57600);
	cfsetospeed(&config_option, B57600);
	tcsetattr(file_descriptor, TCSAFLUSH, &config_option);
	return file_descriptor;
}



int main()
{
  printf("\n---------------------UART LOOPBACK TEST----------------------\n");   	
  int file, cnt_byte = 0;
	file = uart_init();
	usleep(1000000);
	unsigned char send = 'c';
	unsigned char rec;
	tcflush(file, TCOFLUSH);
  cnt_byte = write(file, &send, 1);
  printf("\n%d byte written\n",cnt_byte);
	usleep(10000);
	cnt_byte =  read(file, &rec, 1);
  printf("\n%d byte read\n",cnt_byte);
	usleep(10000);
	tcflush(file, TCOFLUSH);
  if(rec == send)
  {
    printf("\nUART LOOPBACK TEST: SUCCESSFULL\n");
  }
  else
  {
    printf("\nUART LOOPBACK TEST: FAILURE\n");
  }
   
  close(file);
  return 0;
}
