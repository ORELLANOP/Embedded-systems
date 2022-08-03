
#include "tmr4.h"

/* v20.11.26
 * Librer�a para Timer 4. Aqu� se definen los par�metros del Timer 4, 
 * de modo de operar como base de temporizaci�n del m�dulo ECCP3 (EPWM3).
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