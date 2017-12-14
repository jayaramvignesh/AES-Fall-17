/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 6th Dec 2017
*
*   file: UserUART.c
*
*   description: User defined functions tailored to current project using Uart driver lib from Tivaware source code
*
*
*
*****************************************************************************/
#include "UserUART.h"


void UARTtoCOMInit(void)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);
}

int uart_init(){
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_IntMasterEnable();
    GPIOPinConfigure(GPIO_PC4_U7RX);
    GPIOPinConfigure(GPIO_PC5_U7TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    ROM_UARTConfigSetExpClk(UART7_BASE, 120000000, 57600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    /*UARTConfigSetExpClk(UART7_BASE, 120000000, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));*/
    ROM_IntEnable(INT_UART7);
    UARTIntRegister(UART7_BASE, UARTIntHandler);
    ROM_UARTIntEnable(UART7_BASE, UART_INT_RX | UART_INT_RT);
    return 0;
}

void UARTIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = ROM_UARTIntStatus(UART7_BASE, true);
    ROM_UARTIntClear(UART7_BASE, ui32Status);
   /* while(ROM_UARTCharsAvail(UART7_BASE))
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
       // ROM_UARTCharPutNonBlocking(UART7_BASE,
         //                                ROM_UARTCharGetNonBlocking(UART7_BASE));

       // uint8_t data = ROM_UARTCharGetNonBlocking(UART7_BASE);
        UARTprintf("\r\nsent succsefully");
        SysCtlDelay(120000000 / (1000 * 3));
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }*/
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
        UARTprintf("\r\n%d",ui32Count);
        ROM_UARTCharPutNonBlocking(UART7_BASE, *pui8Buffer++);
    }
}
