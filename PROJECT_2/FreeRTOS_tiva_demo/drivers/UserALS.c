/*************************************************************************
*   Authors: Arundhathi Swami and Vignesh Jayaram
*   date edited: 8th Dec 2017
*
*   file: UserALS.c
*
*   description: User defined functions tailored to MQ3 alcohol sensor initialization and reading using
*    Tivaware source code
*
*
*
*****************************************************************************/


#include "UserALS.h"

float calibrateALS(void)
{
    ALSADCInit();
    float alsvolt = 0.0;
    float alsair = 0.0;
    float ratio = 0.0;
    char cratio[10];
    uint32_t alsvalue[1] = {0.0};
    float cumulativeval = 0.0;
    int i =0;
    for(i =0; i<100; i++)
    {
        /*Trigger ADC conversion*/
        ADCProcessorTrigger(ADC0_BASE, 3);

        /*Wait for conversion to complete*/
        while(!ADCIntStatus(ADC0_BASE, 3, false));

        /*Clear ADC interrupt flag*/
        ADCIntClear(ADC0_BASE, 3);

        /*Read ADC value*/
        ADCSequenceDataGet(ADC0_BASE, 3, alsvalue);

        cumulativeval += (float)alsvalue[0];
        memset(alsvalue, 0, sizeof(alsvalue));

     }
    cumulativeval = cumulativeval/100.0;
    alsvolt = (cumulativeval/1024)*5.0;
    alsair = (5.0 - alsvolt)/alsvolt;
    ratio = alsair/60.0;
    sprintf(cratio, "%f", ratio);
    UARTprintf("Clean Air Constant: %s\n", cratio);
    return ratio;
}
