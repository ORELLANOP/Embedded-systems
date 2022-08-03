
#include "tmr1.h"

/* v20.11.26
 * Esta librería maneja al Timer 1 para generar interrupciones fijas cada "10ms".
 * Solo se lo apaga o enciende, de modo que cuente o no las ticks. Se destina al toggleo
 * de el led de "Estado del sistema".
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

volatile uint16_t timer1ReloadVal;

void TMR1_Initialize(void){
    //T1GSS T1G_pin; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO done; T1GSPM disabled; 
    T1GCON = 0x00;
    /* Registros de cuenta configurados para overflow en 10ms */
    TMR1H = 0xB1;
    TMR1L = 0xE0;
    timer1ReloadVal= (TMR1H << 8) + TMR1L;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    // T1CKPS 1:8; T1OSCEN disabled; T1SYNC synchronize; TMR1CS FOSC/4; TMR1ON disabled; T1RD16 enabled; 
    T1CON = 0x32;
    return;
}

void TMR1_StartTimer(void){    
    T1CONbits.TMR1ON = 1;
    return;
}

void TMR1_StopTimer(void){
    T1CONbits.TMR1ON = 0;
    return;
}

void TMR1_WriteTimer(uint16_t timerVal){
    if (T1CONbits.T1SYNC == 1){
        T1CONbits.TMR1ON = 0;
        TMR1H = (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;
        T1CONbits.TMR1ON =1;
    }
    else{
        TMR1H = (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;
    }
    return;
}

void TMR1_Reload(void){
    TMR1_WriteTimer(timer1ReloadVal);
    return;
}

void TMR1_ISR(void){
    PIR1bits.TMR1IF = 0;
    TMR1_Reload();
    /* Debe escribirse lo necesario para el LED de estado */
    return;
}