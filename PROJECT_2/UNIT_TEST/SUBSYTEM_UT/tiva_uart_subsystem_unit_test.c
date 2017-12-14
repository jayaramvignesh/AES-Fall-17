/*******************************************************
 * FreeRTOS 8.2 Tiva Demo
 *
 *  tiva_uart_subsystem_unit_test.c
 *
 *  Author: Arundhathi Swami and Vignesh Jayaram
 *
 *  The TIVA SIDE UART has been initiazlized and sending 
 *  structure packet to the BeagbleBone Side
 *
 *******************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include <time.h>
#include <stdlib.h>

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "drivers/comm.h"

struct packet
{
    uint8_t log_id;
    uint8_t log_level;
    float data;
    char timestamp[25];
    char c;
};


// Main function
int main(void)
{

    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);
   
   /*Initialzing UART*/
    uart_init();

    struct packet p1;
    unsigned long int i = 0;
    unsigned long int j = 0;
    uint32_t count = 0;
   
   /*sending for three different cases*/
   do
   {
		/*clearing the structure*/
        memset(&p1,0,sizeof(p1));

		/*forming the packet*/
        time_t a = time(NULL);
        char *ptr = ctime(&a);
        strcpy(p1.timestamp,ptr);
        count++;
        
		/*checking for value of count*/
		if(count == 1)
        {
            p1.log_level = rand()%4;
            p1.data = 0;
            p1.log_id = rand()%6;
        }
        else if(count == 2)
        {
            p1.data = (float)((-1.1)*(rand()%(1000)));
            p1.log_level = 1;
            p1.log_id = 3;
        }
        else if(count == 3)
        {
            p1.data = (rand()%(10));
            p1.log_level = 1;
            p1.log_id =  4;
            count = 0;
        }
        p1.c = '\0';
        
		/*send the data*/
		UARTSend((uint8_t *)&p1,sizeof(p1));
       
	    /*Delay*/
		for(i = 0; i<100000;i++)
        {
           for(j = 0; j<1000; j++)
           {
           }
        }

        i = 0;
   }while(count);
}


/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}