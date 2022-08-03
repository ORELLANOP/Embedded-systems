
#include "tmr4.h"

/* v20.11.26
 * Librería para Timer 4. Aquí se definen los parámetros del Timer 4, 
 * de modo de operar como base de temporización del módulo ECCP3 (EPWM3).
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

void TMR4_StartTimer(void) {
    T4CONbits.TMR4ON = 1;
    return;
}

void TMR4_StopTimer(void) {
    T4CONbits.TMR4ON = 0;
    return;
}

void TMR4_WriteTimer(uint8_t timerVal) {
    TMR4 = timerVal;
    return;
}

void TMR4_LoadPeriodRegister(uint8_t periodVal) {
    PR4 = periodVal;
    return;
}