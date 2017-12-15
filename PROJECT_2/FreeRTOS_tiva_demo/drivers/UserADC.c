/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserADC.c
*
*   description: User defined functions tailored to current project using adc and gpio driver lib from Tivaware source code
*
*
*
*****************************************************************************/


#include "UserADC.h"

void ALSADCInit(void)
{

    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlDelay(10);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);    //ADC Channel-0 for the echo pin of USS Only


    /* Enable sample sequence 3 with a processor signal trigger.  Sequence 3 will do a single sample when the processor sends a signal to start the
     conversion.  Each ADC module has 4 programmable sequences, sequence 0 to sequence 3.ADCSequenceConfigure(ADC0_BASE, 3,
     ADC_TRIGGER_PROCESSOR, 0);*/

    /*Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    single-ended mode (default) and configure the interrupt flag
    (ADC_CTL_IE) to be set when the sample is done in sequence 3 - one step conversion */
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

    /*Clear the interrupt status flag to avoid false triggers*/
    ADCIntClear(ADC0_BASE, 3);

    /* Enable Sequence 3.*/
    ADCSequenceEnable(ADC0_BASE, 3);
}
