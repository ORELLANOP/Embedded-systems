
#include "tmr2.h"

/* v20.11.26
 * Librería para Timer 2. Aquí se definen los parámetros del Timer 2, 
 * de modo de operar como base de temporización del módulo ECCP2 (EPWM2).
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

void TMR2_StartTimer(void) {
    T2CONbits.TMR2ON = 1;
    return;
}

void TMR2_StopTimer(void) {
    T2CONbits.TMR2ON = 0;
    return;
}

void TMR2_WriteTimer(uint8_t timerVal) {
    TMR2 = timerVal;
    return;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal) {
    PR2 = periodVal;
    return;
}