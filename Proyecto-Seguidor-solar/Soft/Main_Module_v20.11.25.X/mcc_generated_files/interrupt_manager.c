
#include "interrupt_manager.h"

/* v20.11.30
 * Librería diseñada para la configuración y manejo de los periféricos y módulos
 * del procesador que trabajan mediante interrupciones. 
 * Las interrupciones externas se configuran en esta librería. Las de los periféricos
 * como la EUSART se configuran en su propia librería.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

void INTERRUPT_Initialize(void) {
    RCONbits.IPEN = 1; // Habilita los dos vectores de interrupción
    /* Prioridades de EUSART 1 
     */
    IPR1bits.RC1IP = 1; // Recepción de USART1 "Alta Prioridad"
    IPR1bits.TX1IP = 0; // Transmisión de USART1 "Baja Prioridad"
    /* Configuración de interrupciones externas "INTx"
     */
    EXT_INT_init();
    /* Prioridad de Timers
     */
    IPR2bits.TMR3IP = 0;
    IPR1bits.TMR1IP = 0;
    /* Borro el flag del IOC que no se utiliza, pero para que no moleste
     */
    INTCONbits.RBIF = 0;
    return;
}

void EXT_INT_init(void) {

    /* Por pin INT0 se levanta al pin MFP del RTCC... 
     */
    INTCONbits.INT0IF = 0;
    INTCON2bits.INTEDG0 = 1; // pin INT0 interrumpe por rising edge
    INTCONbits.INT0IE = 1; /* habilitación de la interrupción por pin INT0.
                            * Esta interrupción es siempre de alta prioridad
                            */
    /* Por pin INT1 para levantar al comparador del motor de elevación "Engine 2"... 
     */
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 1; // Alta prioridad    
    INTCON2bits.INTEDG1 = 1; // pin INT1 interrumpe por rising edge
    INTCON3bits.INT1IE = 1; // interrupción INT1 habilitada

    /* Por pin INT2 para levantar al pulsador de cambio de modo "BUT_MODE"... 
     */
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IP = 0; // Baja prioridad
    INTCON2bits.INTEDG2 = 0; // Interrumpe for falling edge
    INTCON3bits.INT2IE = 1; // interrupción INT2 habilitada 

    MODE_BUTTON = NO_CHANGE;
    return;
}

void INT0_ISR(void) {
    /* Configuración del pin manejado por IOC para levantar la señar MFP
     * del RTCC.
     */
    INTCONbits.INT0IF = 0;
    return;
}

void INT1_ISR(void) {
    /* Si se detecta interrupción por INT1...*/
    EPWM3_STOP();
    INTCON3bits.INT1IF = 0;
    return;
}

void INT2_ISR(void) {
    /* Función de interrupción para cambio de modo*/
    if (MODE_BUTTON < AUTO_MOD)
        MODE_BUTTON++;
    else
        MODE_BUTTON = MANUAL_MOD;
    /* Agregar un sondeo de la variable MODE_BUTTON en "user", "autonomous" y "demo"
     * o a donde haga falta, para ver si hay un cambio de modo actual. */
    INTCON3bits.INT2IF = 0;
    return;
}

void __interrupt(high_priority) INTERRUPT_InterruptManagerHigh(void) {
    if (INTCON3bits.INT1IE == 1 && INTCON3bits.INT1IF == 1) {
        INT1_ISR();
    } else if (PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1) {
        EUSART1_RxDefaultInterruptHandler();
    } else if (INTCONbits.INT0IE == 1 && INTCONbits.INT0IF == 1) {
        INT0_ISR();
    }
    return;
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow(void) {
    if (INTCON3bits.INT2IE == 1 && INTCON3bits.INT2IF == 1) {
        INT2_ISR();
    } else if (PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1) {
        EUSART1_TxDefaultInterruptHandler();
    } else if (PIE2bits.TMR3IE == 1 && PIR2bits.TMR3IF == 1) {
        TMR3_ISR();
    } else if (PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF == 1) {
        TMR1_ISR();
    }
    return;
}