
#include "tmr0.h"

/* v20.09.18
 * Librería para Timer 0. Este Temporizador se utiliza para generar los arranques suaves y 
 * detenciones suaves de los motores. Para ello el timer puede configurarse entre 5ms
 * y 131ms (prescaler en 1:32), con el fin de que en cada paso de dicho tiempo, 
 * se aumente o disminuta según corresponda, un "1%" en el duty del motor que se esté operando. 
 * También se utiliza esta librería como "whach dog" del i2c2 en caso de no respuesta.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

volatile uint16_t timer0ReloadVal;
volatile uint8_t T;

void TMR0_Initialize(uint8_t period) { // "Period" en milisegundos           
    T0CON = 0x04; // Timer Off, T16bits, Presc 1:32
    INTCONbits.TMR0IF = 0;
    /* Cálculo de los registros TMR0H y TMR0L en base a "period". Luego se carga los valores de ambos
     * registros, de modo concatenado en la variable timer0ReloadVal, para que el timer pueda ser recargado
     * en cada desborde.    
     */
    timer0ReloadVal = TMR0_WriteTimer(period);
    T = period; // periodo en milisegundos
    return;
}

void TMR0_period(uint8_t per) {
    timer0ReloadVal = TMR0_WriteTimer(per);
    T = per;
    return;
}

uint8_t TMR0_request_period(void) {
    return T;
}

void TMR0_StartTimer(void) {
    T0CONbits.TMR0ON = 1;
    return;
}

void TMR0_StopTimer(void) {
    T0CONbits.TMR0ON = 0;
    return;
}

uint16_t TMR0_WriteTimer(uint8_t timerVal) {
    uint16_t period;
    if (timerVal < PERIOD_MIN)
        period = UINT16_MAXVALUE - ((PERIOD_MIN * FOSC_kHz) / (4 * 32));
    else if (timerVal > PERIOD_MAX)
        period = UINT16_MAXVALUE - ((PERIOD_MAX * FOSC_kHz) / (4 * 32));
    else
        period = UINT16_MAXVALUE - ((timerVal * FOSC_kHz) / (4 * 32));

    TMR0H = period >> 8;
    TMR0L = (uint8_t) period;
    return (uint16_t) ((period << 8) | period);
}

void TMR0_Reload(void) {
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;
    return;
}

bool TMR0_HasOverflowOccured(void) {
    return (INTCONbits.TMR0IF);
}