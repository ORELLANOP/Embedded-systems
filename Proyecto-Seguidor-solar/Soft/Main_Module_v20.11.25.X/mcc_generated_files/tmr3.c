
#include "tmr3.h"

/* v20.11.26
 * Esta librería maneja al Timer 3 para generar interrupciones fijas cada "10ms".
 * Solo se lo apaga o enciende, de modo que cuente o no las ticks. Se destina al toggleo
 * de el led de "Errores en el sistema".
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

volatile uint16_t timer3ReloadVal;

void TMR3_Initialize(void) {
    //T3GSS T3G_pin; TMR3GE disabled; T3GTM disabled; T3GPOL low; T3GGO done; T3GSPM disabled; 
    T3GCON = 0x00;
    /* Registros de cuenta configurados para overflow en 10ms */
    TMR3H = 0xB1;
    TMR3L = 0xE0;
    timer3ReloadVal = (TMR3H << 8) + TMR3L;
    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;
    // T3CKPS 1:8; T3OSCEN disabled; nT3SYNC synchronize; TMR3CS FOSC/4; TMR3ON disabled; T3RD16 enabled; 
    T3CON = 0x32;
    return;
}

void TMR3_StartTimer(void) {
    T3CONbits.TMR3ON = 1;
    return;
}

void TMR3_StopTimer(void) {
    T3CONbits.TMR3ON = 0;
    return;
}

void TMR3_WriteTimer(uint16_t timerVal) {
    if (T3CONbits.nT3SYNC == 1) {
        T3CONbits.TMR3ON = 0;
        TMR3H = (timerVal >> 8);
        TMR3L = (uint8_t) timerVal;
        T3CONbits.TMR3ON = 1;
    } else {
        TMR3H = (timerVal >> 8);
        TMR3L = (uint8_t) timerVal;
    }
    return;
}

void TMR3_Reload(void) {
    TMR3_WriteTimer(timer3ReloadVal);
    return;
}

void TMR3_ISR(void) {
    PIR2bits.TMR3IF = 0;
    TMR3_Reload();
    /* Debe escribirse lo necesario para el LED de ERROR */
    return;
}