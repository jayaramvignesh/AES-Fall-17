/*******************************************************
 * FreeRTOS 8.2 Tiva Demo
 *
 *  queue_unit_test_tiva.c
 *
 *  Author: Arundhathi Swami and Vignesh Jayaram
 *
 *  The queue API has been tested for add, remove, items available.
 *  Both positive and negative testing has been done.
 *
 *******************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DATA_SIZE 200

/*Macro for queue*/
#define QUEUE_LENGTH 2

typedef enum queue_return
{
    ADD_SUCCESSFUL = 1,
    REMOVE_SUCCESSFUL = 2,
    ITEMS_REMAINING_SUCCESSFUL = 3,
    ITEMS_REMAINING_FAIl = 4,
    ADD_FAIL = 5,
    REMOVE_FAIL = 6
}q_return;

uint32_t queue_items_remaining_succsefull()
{
    /*Handler for Queue*/
     QueueHandle_t basic_queue;

     char trial[] = "hello world";

     /*Create a Queue*/
     basic_queue = xQueueCreate(QUEUE_LENGTH,sizeof(trial));
     if(basic_queue == NULL)
     {
         UARTprintf("\r\nQueue not be created\r\n");
     }

     return (uxQueueSpacesAvailable(basic_queue));
}

uint32_t queue_items_remaining_fail()
{
    /*Handler for Queue*/
     QueueHandle_t basic_queue;

     char trial[] = "hello world";

     /*Create a Queue*/
     basic_queue = xQueueCreate(QUEUE_LENGTH,sizeof(trial));
     xQueueSendToBack( basic_queue, &trial, 10 );
     xQueueSendToBack( basic_queue, &trial, 10 );
     if(basic_queue == NULL)
     {
         UARTprintf("\r\nQueue not be created\r\n");
     }

     return (uxQueueSpacesAvailable(basic_queue));
}

q_return queue_add_succesfull()
{
    /*Handler for Queue*/
    QueueHandle_t basic_queue;

    char trial[] = "hello world";

    /*Create a Queue*/
    basic_queue = xQueueCreate(QUEUE_LENGTH,sizeof(trial));
    if(basic_queue == NULL)
    {
      UARTprintf("\r\nQueue not be created\r\n");
    }


    if( xQueueSendToBack( basic_queue, &trial, 10 ) != pdPASS )
    {
       return ADD_FAIL;
    }
    else
    {
        return ADD_SUCCESSFUL;
    };
}


q_return queue_remove_succesfull()
{
    /*Handler for Queue*/
    QueueHandle_t basic_queue;

    char trial[] = "hello world";
    char receiver[12] = {};

    /*Create a Queue*/
    basic_queue = xQueueCreate(QUEUE_LENGTH,sizeof(trial));
    if(basic_queue == NULL)
    {
      UARTprintf("\r\nQueue not be created\r\n");
    }

    xQueueSendToBack( basic_queue, &trial, 10 );

    if( xQueueReceive( basic_queue, &receiver, 10) != pdPASS )
    {
       return REMOVE_FAIL;
    }
    else
    {
        return REMOVE_SUCCESSFUL;
    };
}

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

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);

    UARTprintf("\r\n------------TIVA QUEUE TESTING-----------------\r\n");

    if((queue_add_succesfull()) == ADD_SUCCESSFUL)
    {
        UARTprintf("\r\nQUEUE ADD UNIT TEST FOR SUCCESS: PASS\r\n");
    }

    if((queue_remove_succesfull()) == REMOVE_SUCCESSFUL)
    {
        UARTprintf("\r\nQUEUE REMOVE UNIT TEST FOR SUCCESS: PASS\r\n");
    }

    if((queue_items_remaining_succsefull()) == QUEUE_LENGTH);
    {
        UARTprintf("\r\nQUEUE ITEMS REMAINING UNIT TEST FOR SUCCESS: PASS\r\n");
    }

    if((queue_items_remaining_fail()) != QUEUE_LENGTH);
    {
        UARTprintf("\r\nQUEUE ITEMS REMAINING UNIT TEST FOR FAIL: PASS\r\n");
    }


    return 0;
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 *
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
