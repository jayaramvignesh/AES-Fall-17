/* Designated Driver Project
 *
 * main.c
 *
 * Author:: Arundhathi Swami, Vignesh Jayaram
 *
 * REF Credits: Andy Kobyljanec
 *
 * This is a project that combines data from three sensors to determine whether a person is capable of driving or not
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

/*PIN CONNECTIONS
 * 1. ALcohol Sensor - Pin 4: PE3
 *                   - Power = 5v
 * 2. Gyroscope - CS - Vcc - 5v
 *              - INT1 - PC7
 *              - SDA - PG1
 *              - SCL - PG0
 * 3. Ultrasonic Sensor - Power - 5v
 *                      - Trig - PC6
 *                      - Echo - 1KOhm - PL3
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"


// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*user defined libraries included*/
#include "drivers/UserTimer.h"
#include "drivers/UserADC.h"
#include "drivers/UserI2CLib.h"
#include "drivers/UserUART.h"
#include "drivers/UserALS.h"
#include "drivers/UserADXL.h"
#include "drivers/UserUSS.h"



/* macro definitions for various tasks*/
#define QUEUE_LENGTH 10

/*heartbeat variables*/
uint8_t exit_notif = 0;

//global variable declaration
int long rawdistance[1];
uint32_t distancecounter = 0;

char dist[8];
uint8_t adxlvalues[10] = {0};
int x = 0, y = 0, z = 0;
double xg = 0, yg = 0, zg = 0;
#define ACTIVITY 90.0
#define INACTIVITY 95.0


float cleangasconstant = 0;
uint32_t alcraw[1];
float alcvoltvalue = 0.0;
float alcgasvalue = 0.0;
float alcratio = 0.0;
char printalc[10];


// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);
void USSTask(void *pvParameters);
void ALSTask(void *pvParameters);
void ADXLTask(void *pvParameters);
void UARTSendTask(void *pvParameters);
void HBTickTask(void *pvParameters);

void vTimerCallback(void);

/*Queue API*/
QueueHandle_t send_queue;

/*Timer API*/
TimerHandle_t xTimer;

/*Semaphore API*/
xSemaphoreHandle queue_mutex=0;
xSemaphoreHandle als_tick=0;
xSemaphoreHandle adxl_tick=0;
xSemaphoreHandle uss_tick=0;
xSemaphoreHandle uart_tick=0;

/*Task notify API initializations*/
static TaskHandle_t xTaskLEDS = NULL, xTaskUSS = NULL, xTaskALS = NULL, xTaskADXL = NULL, xTaskUART = NULL;



void vTimerCallBack(void* a){
    /*Signal ADXL TASK*/
    xTaskNotifyGive( xTaskALS);
    xTaskNotifyGive( xTaskADXL);
    xTaskNotifyGive( xTaskUSS);
 //   xTaskNotifyGive( xTaskUART);
    xTaskNotifyGive( xTaskLEDS);
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

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    /*configure uart*/
    UARTtoCOMInit();
    UARTprintf("\n ---- WELCOME ----\n", dist);
    uart_init();

    /*configure USS*/
    /*set up gpio*/
    USSGPIOInit();


    /*initialise accelerometer*/
    ADXLSInit();

    /*Set up adc 0 channel 3 - pin e-3*/
    cleangasconstant = calibrateALS();
    char cac[10];
    sprintf(cac, "%f", cleangasconstant);
    UARTprintf("Clean Air Ratio in Main: %s\n\n\n\n", cac);
    //COSADCInit();

    SysCtlDelay(120000); //Delay by 10us

    //Create a Queue
    send_queue = xQueueCreate(QUEUE_LENGTH,sizeof(send_pkt));
        if(send_queue == NULL){
            UARTprintf("\r\nQueue not be created\r\n");
        }

    /* initializing synchronization devices*/
    vSemaphoreCreateBinary(als_tick);
    vSemaphoreCreateBinary(adxl_tick);
    vSemaphoreCreateBinary(uss_tick);
    vSemaphoreCreateBinary(uart_tick);

    queue_mutex=xSemaphoreCreateMutex();

    // Create tasks
    xTaskCreate(HBTickTask, (const portCHAR *)"HeartBeat",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    xTaskCreate(ALSTask, (const portCHAR *)"ALS",
                    configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskALS);

    xTaskCreate(ADXLTask, (const portCHAR *)"ADXL",
                   configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskADXL);

    xTaskCreate(USSTask, (const portCHAR *)"USS",
                    configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskUSS);

    xTaskCreate(UARTSendTask, (const portCHAR *)"UARTtoBBG",
                    configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskUART);

    xTaskCreate(demoLEDTask, (const portCHAR *)"LEDs",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskLEDS);

    //initialize timer for 2 secs
        xTimer = xTimerCreate("SyncTimer",2000,pdTRUE,(void*)0,vTimerCallBack);
        if(xTimer==NULL)
        {
           UARTprintf("\r\nERROR: Timer Creation Failed\r\n");
        }
        else
        {
           if(xTimerStart(xTimer,0)!=pdPASS)
           {
               UARTprintf("\r\nERROR: Timer Start Failed\r\n");
           }
        }

        vTaskStartScheduler();
    return 0;
}

void HBTickTask(void *pvParameters)
{
    UARTprintf("\r\nHeartBeat monitoring Started\n");
    for ( ; ; )
    {
        /*wait for jeartbeat from als Task*/
        if(xSemaphoreTake(als_tick,15000) != pdPASS)
        {
            UARTprintf("\r\nHeartbeat for alcohol sensor task failed\r\n");
            exit_notif=1;
        }

        if(xSemaphoreTake(adxl_tick,15000) != pdPASS)
        {
            UARTprintf("\r\nHeartbeat for accelerometer task failed\r\n");
            exit_notif=1;
        }

        if(xSemaphoreTake(uss_tick,15000)!=pdPASS)
        {
            UARTprintf("\r\nHeartbeat for Ultrasonic Sensor task failed\r\n");
            exit_notif=1;
        }

        if(xSemaphoreTake(uart_tick,15000)!=pdPASS)
        {
            UARTprintf("\r\nHeartbeat for Uart Sending task failed\r\n");
            exit_notif=1;
        }

        if(exit_notif==1)
        {
            break;
        }

    }
}


void ALSTask(void *pvParameters)
{
    send_pkt alspkt;
    for ( ; ;)
    {

        /*Send heartbeat notif to the heartbeat task*/
        xSemaphoreGive(als_tick);

        /*check if exit flag is set*/
        if(exit_notif == 1)
        {
            vTaskDelete(NULL);
        }

        /* Block to wait on LEDs Task */
        if(ulTaskNotifyTake( pdTRUE, portMAX_DELAY ))
        {
            if(xSemaphoreTake(queue_mutex,portMAX_DELAY))
            {
                memset(&alspkt, 0, sizeof(send_pkt));
                /*Trigger ADC conversion*/
                ADCProcessorTrigger(ADC0_BASE, 3);

                /*Wait for conversion to complete*/
                while(!ADCIntStatus(ADC0_BASE, 3, false));

                /*Clear ADC interrupt flag*/
                ADCIntClear(ADC0_BASE, 3);

                /*Read ADC value*/
                ADCSequenceDataGet(ADC0_BASE, 3, alcraw);

                /*convert*/
                alcvoltvalue = (float)alcraw[0]/1024.0 * 5.0;
                alcgasvalue = (5.0 - alcvoltvalue)/alcvoltvalue;
                alcratio = alcgasvalue/cleangasconstant;
                alspkt.data = alcratio;

                /*print the alcohol content*/
                memset(printalc, 0, sizeof(printalc));
                sprintf(printalc, "%f", alcratio);

                /* Send the message to the queue, waiting for 10 ticks for space to become available if the queue is already full. */

                       alspkt.log_id = 4;
                        alspkt.log_level = 1;
                        alspkt.c = '\0';
                        time_t t = time(NULL);
                        char* times = ctime(&t);
                        strcpy(alspkt.timestamp, times);
                if( xQueueSendToBack( send_queue, &alspkt, 10 ) != pdPASS )
                {
                    UARTprintf("\r\nERROR: Queue is full\r\n");
                }
                /*
                 * 280 - sober
                 * 280-350 - 1 beer
                 * 350- 450 - few beers
                 * 450 above - get outa the fucking car
                 */

                 UARTprintf("Alcohol Content = %s\n", printalc);
                 //vTaskDelay(1000);

            }//mutex
            /*release queue mutex*/
            xSemaphoreGive(queue_mutex);


        }//semaphore
    }//for loop
}//function loop

//accelerometer values read from sensor
void ADXLTask(void *pvParameters)
{
    uint8_t intsrc;
    float movement = 0.0;
    send_pkt adxlpkt;
    for( ; ;)
    {


        /*Send heartbeat notif to the heartbeat task*/
        xSemaphoreGive(adxl_tick);

        /*check if exit flag is set*/
        if(exit_notif == 1)
        {
            vTaskDelete(NULL);
        }

        /* Block to wait on ALS Task */
        if (ulTaskNotifyTake( pdTRUE, portMAX_DELAY ))
        {
            /*block to wait on mutex*/
            if(xSemaphoreTake(queue_mutex,portMAX_DELAY))
            {
                intsrc = 0;
                memset(adxlvalues, 0, sizeof(adxlvalues));

                adxlvalues[0] = readReg(DEVADDR,DATAX0);
                adxlvalues[1] = readReg(DEVADDR,DATAX1);
                adxlvalues[2] = readReg(DEVADDR,DATAY0);
                adxlvalues[3] = readReg(DEVADDR,DATAY1);
                adxlvalues[4] = readReg(DEVADDR,DATAZ0);
                adxlvalues[5] = readReg(DEVADDR,DATAZ1);
                intsrc = readReg(DEVADDR, INTSOURCE); //Clear the interrupts from the INT_SOURCE register.
                if(intsrc == 0x2)
                {
                    UARTprintf("Sensed Interrupts: INACTIVITY\n");
                    movement = INACTIVITY;
                }

                else
                {
                    UARTprintf("Sensed Interrupts: ACTIVITY\n");
                    movement = ACTIVITY;
                }
                memset(&adxlpkt, 0, sizeof(send_pkt));
                        adxlpkt.log_id = 3;
                        adxlpkt.log_level = 1;
                        adxlpkt.c = '\0';
                        time_t t = time(NULL);
                        char* times = ctime(&t);
                        strcpy(adxlpkt.timestamp, times);
                adxlpkt.data = movement;
                /* Send the message to the queue, waiting for 10 ticks for space to become available if the queue is already full. */
                if( xQueueSendToBack( send_queue, &adxlpkt, 10 ) != pdPASS )
                {
                    UARTprintf("\r\nERROR: Queue is full\r\n");
                }

#ifdef PRINTG
                x = (((int)adxlvalues[1] <<  8) | (int)adxlvalues[0]);
                y = (((int)adxlvalues[3] <<  8) | (int)adxlvalues[2]);
                z = (((int)adxlvalues[5] <<  8) | (int)adxlvalues[4]);
                xg = x * 0.0078;
                yg = y * 0.0078;
                zg = z * 0.0078;
//              UARTprintf("Sensed Values = x:%d, y:%d, z:%d\n", xg, yg, zg);
#endif
        //vTaskDelay(1000);
                /*release queue mutex*/
                xSemaphoreGive(queue_mutex);
            }//queue mutex
        }//notif
    }//for
}//function


//get readings from Ultrasonic Sensor
void USSTask(void *pvParameters)
{
    /*code to trigger input for 10us and read echo output*/
    send_pkt usspkt;
    for( ; ; )
    {
        static float distance = 0;

        /*Send heartbeat notif to the heartbeat task*/
        xSemaphoreGive(uss_tick);

        /*check if exit flag is set*/
        if(exit_notif == 1)
        {
           vTaskDelete(NULL);
        }

        /*block on ADXL Task*/
        if(ulTaskNotifyTake( pdTRUE, portMAX_DELAY ))
        {
            /*block to wait on mutex*/
            if(xSemaphoreTake(queue_mutex,portMAX_DELAY))
            {
                /*trigger*/
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, ~(GPIO_PIN_6));
                // delayUs(10); //10usDelay
                SysCtlDelay(1200); //Delay by 10us

                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);
                // delayUs(10); //10usDelay
                SysCtlDelay(1200); //Delay by 10us

                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, ~(GPIO_PIN_6));

                /*read echo*/
                distancecounter = 0;
                while(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_3) == 0)
                {

                }
                while ((GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_3) != 0) & (distancecounter<MAX_TIME))
                {
                   distancecounter++;
                   SysCtlDelay(120); //Delay by 1us

                  // delayUs(1); //1usDelay
                }

                /*convert*/
                distance = ((float)distancecounter/(float)58.2);
                distance = distance *6;
                memset(&usspkt, 0, sizeof(send_pkt));
                        usspkt.log_id = 6;
                        usspkt.log_level = 1;
                        usspkt.c = '\0';
                        time_t t = time(NULL);
                        char* times = ctime(&t);
                        strcpy(usspkt.timestamp, times);
                usspkt.data = distance;
                /* Send the message to the queue, waiting for 10 ticks for space to become available if the queue is already full. */
                if( xQueueSendToBack( send_queue, &usspkt, 10 ) != pdPASS )
                {
                    UARTprintf("\r\nERROR: Queue is full\r\n");
                }

                sprintf(dist, "%f", distance);
//                UARTSend((uint8_t *)&usspkt, sizeof(usspkt));
//                uint32_t i = 0;
//                for (i = 0; i<10000; i++);
                /*print the distance in cms*/
                UARTprintf("distance = %s cm\n", dist);

                /*release queue mutex*/
                xSemaphoreGive(queue_mutex);
            }
        }
    }
}

void UARTSendTask(void *pvParameters)
{
    uart_init();
    send_pkt recpkt;
    for( ; ;)
    {
        /*Send heartbeat notif to the heartbeat task*/
        xSemaphoreGive(uart_tick);
        //UARTprintf("\r\nIn receive task to receive data\r\n");
        /* Block to wait on LEDs Task */
      //  if(ulTaskNotifyTake( pdTRUE, portMAX_DELAY ))
       // {

            while(uxQueueSpacesAvailable(send_queue) != QUEUE_LENGTH)

            {
                if(xSemaphoreTake(queue_mutex,portMAX_DELAY))
                {
                    //UARTprintf("\r\nIn receive task to receive data\r\n");
                    memset((void*)&recpkt, 0, sizeof(send_pkt));
                    if( xQueueReceive( send_queue, &recpkt, portMAX_DELAY ) != pdPASS )
                    {
                        UARTprintf("\r\ERROR:Queue Receive\r\n");
                    }
                    else
                    {
                        uint32_t i = 0;
                        char tp[10];
                        UARTprintf("\n/*-------------------------*/\n");
                        UARTprintf("Log ID: %d\n",recpkt.log_id);
                        UARTprintf("Log LEVEL: %d\n",recpkt.log_level);
                        UARTprintf("TIMESTAMP: %s, %d\n",recpkt.timestamp, strlen(recpkt.timestamp));
                        sprintf(tp, "%f", recpkt.data );
                        UARTprintf("DATA: %s\n", tp);
                        UARTprintf("/*-------------------------*/\n");

                       UARTSend((uint8_t *)&recpkt, sizeof(recpkt));
                       for (i = 0; i<10000; i++);

                    }

                }//mutex
                xSemaphoreGive(queue_mutex);
            }//while

            /*check if exit flag is set*/
            if(exit_notif == 1)
            {
                vTaskDelete(NULL);
            }
       // }

    }

}

//#ifdef DEBUG
// Flash the LEDs on the launchpad
void demoLEDTask(void *pvParameters)
{
    for (;;)
    {
        /* Block to wait on LEDs Task */
        if(ulTaskNotifyTake( pdTRUE, portMAX_DELAY ))
        {
            // Turn on LED 1
            LEDWrite(0x0F, 0x01);
            vTaskDelay(1000);
            LEDWrite(0x00, 0x01);

        }

    }
}


// Write text over the Stellaris debug interface UART port
void demoSerialTask(void *pvParameters)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);


    for (;;)
    {
        UARTprintf("\r\nHello, world from FreeRTOS 9.0!");
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
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
