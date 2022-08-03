
#include "epwm3.h"

/* v20.11.30
 * Librer�a para el manejo del m�dulo PWM (ECCP3) del PIC18F46K22, destinando
 * su dise�o al puente L298. 
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Esta librer�a solo requiere ubicar en el "pin_manager.h y .c", o directamente
 * desde la paleta del MCC un pin llamado ENB (Enable B), el cual es utilizado
 * para habilitar o no el canal f�sico del m�dulo L298
 */

uint8_t count_EPWM3_SStart, count_EPWM3_SStop, interval_ISR;

void EPWM3_Initialize(uint8_t freq) {
    TRISE |= 0x03; /* Configuro como entradas los pines RE0-RE1, durante el proceso
                    * de SETUP, para evitar se�ales espurias. 
                    */
    ENB_LAT = LOW;
    ENB_TRIS = INPUT; /* Configuro como entrada el ENB durante el setup para L298 */
    PSTR3CON = 0x13; /* Las salidas de ECCP2_PWM ocurren en el siguiente per�odo 
                      * al primero del Timer. P3A y P3B se encuentran habilitados 
                      * para funcionar como salida PWM.
                      */
    CCPTMRS0bits.C3TSEL = 0b01; /* El m�dulo de PWM utiliza como base de tiempo a Timer 4 */
    if (freq < FREC_MIN)
        EPWM3_period(FREC_MIN);
    else
        EPWM3_period(freq);
    ECCP3ASbits.CCP3AS = 0; /* Auto-shutdown (auto apagado) deshabilitado para ECCP3 */
    ECCP3ASbits.PSS3AC = 0; /* Pins PxA and PxC Shutdown State Control bits  
                             * 00 = Pin PxA a "cero" en estado shutdown*/
    ECCP3ASbits.PSS3BD = 0; /* Pins PxB and PxD Shutdown State Control bits
                             * 00 = Pin PxB a "cero" en estado shutdown */
    PWM3CONbits.P3RSEN = 0; /* Auto reinicio deshabilitado. En caso de que ocurra un evento
                             * de shutdown, el bit ASE del registro ECCPxAS, debe limpiarse
                             * por software (Restart Manual por firmware)
                             */
    CCP3CONbits.P3M = 0b10; /* Half-Bridge ECCP3 Module, con P3A y P3B moduladas con 
                             * tiempo de banda muerta.
                             */
    CCP3CONbits.CCP3M = 0b1100; /* Half-Bridge ECCP3 Module. 1100 = PWM mode: 
                                 * P3A active-high; P3B active-high 
                                 * if CCPxM<3:2> = 11: (PWM modes) 
                                 */
    EPWM3_engine_config(NONE, 50); /* Se config. con duty 50%, es decir sin giro */

    PWM3CONbits.P3DC = 0x50; /* PWM Delay Count bits (Dead-Band Delay Mode)) 
                              * configurado para 5us de delay 0x50 = 80  
                              * (Esto genera un "td = 5us") 
                              */
    T4CONbits.T4CKPS = 0b11; /* valor del divisor de la frec de entrada 
                              * (Fosc / 4) -> prescaler = 16 
                              */
    TMR4_StopTimer(); /* Apaga el Timer 4 */
    TMR4_WriteTimer(0);
    TRISE &= 0xFC; /* Configuro como salida los pines RE0-RE1, para P3A y P3B */
    ENB_LAT = LOW;
    ENB_TRIS = OUTPUT; /* Configuro como salida el RC2, pin ENB  para L298 */
    EPWM3_outputs_off();
    STATUS_Eng_V.status = INACTIVE_ENGINE;
    STATUS_Eng_V.turn = NONE;
    return;
}

void EPWM3_period(uint8_t freq) {
    static uint8_t freq_lure = 0;
    if (freq_lure != freq) {
        if (freq <= FREC_MIN)
            TMR4_LoadPeriodRegister((FOSC_kHz / (FREC_MIN * 4 * 16)) - 1);
        else
            TMR4_LoadPeriodRegister((FOSC_kHz / (freq * 4 * 16)) - 1);
        freq_lure = freq;
        EPWM3_duty_const(freq);
    }
    return;
}

float EPWM3_duty_const(uint8_t freq) {
    static float interval = 0;
    if (freq != REQUEST_DUTY_CONST) {
        interval = (float) FOSC_kHz / (freq * 100 * 16);
        interval_ISR = interval;
    }
    return interval;
}

void EPWM3_engine_config(uint8_t turn, uint8_t duty) {
    float interval = EPWM3_duty_const(REQUEST_DUTY_CONST);
    uint16_t pulse_width_bit;
    if (turn == CONTRACT || turn == NONE) {
        pulse_width_bit = duty * interval;
        CCPR3L = pulse_width_bit >> 2;
        CCP3CONbits.DC3B = pulse_width_bit;
    } else if (turn == TO_EXTEND) {
        pulse_width_bit = (100 - duty) * interval;
        CCPR3L = pulse_width_bit >> 2;
        CCP3CONbits.DC3B = pulse_width_bit;
    }
    return;
}

void EPWM3_outputs_off(void) {
    /* Shutdown por firmware */
    ECCP3ASbits.CCP3ASE = 1;
    return;
}

void EPWM3_outputs_on(void) {
    ECCP3ASbits.CCP3ASE = 0;
    return;
}

void EPWM3_engine_running(void) {
    EPWM3_outputs_on();
    TMR4_WriteTimer(0);
    TMR4_StartTimer();
    ENB_LAT = HIGH;
    return;
}

void EPWM3_engine_off(void) {
    if (STATUS_Eng_V.status != INACTIVE_ENGINE) {
        ENB_LAT = LOW;
        EPWM3_outputs_off();
        TMR4_StopTimer();
        STATUS_Eng_V.status = INACTIVE_ENGINE; // motor detenido        
        STATUS_Eng_V.turn = NONE;
    }
    return;
}

/* Funciones especiales para mover el Motor 2 con suavidad en el arranque 
 * y apara detenerlo con suavidad cuando deba cortar su funcionamiento 
 */

void EPWM3_engine_SoftStart(uint8_t turn, uint8_t dut_max) {
    if (STATUS_Eng_V.status == INACTIVE_ENGINE) { // Si el motor est� inactivo   
        /* Durante el arranque suave deshabilito la interrupci�n de protecci�n contra 
         * sobrecorriente
         */
        INTCON3bits.INT1IE = 0;
        STATUS_Eng_V.turn = turn;
        STATUS_Eng_V.status = STARTING_ENGINE; // Motor en proceso de arranque        
        /* Accionamiento sobre el motor */
        count_EPWM3_SStart = STOPPED_ENGINE;
        EPWM3_engine_config(turn, count_EPWM3_SStart);
        if (TMR0_request_period() != TMR0_Start2)
            TMR0_period(TMR0_Start2);
        else
            TMR0_Reload();
        EPWM3_engine_running();
        TMR0_StartTimer();
        do {
            while (!TMR0_HasOverflowOccured());
            INTCONbits.TMR0IF = 0;
            TMR0_Reload();
            EPWM3_engine_config(turn, --count_EPWM3_SStart);
        } while (count_EPWM3_SStart >= dut_max);
        count_EPWM3_SStop = dut_max;
        STATUS_Eng_V.status = ACTIVE_ENGINE; // motor en marcha
        TMR0_StopTimer();
        /* Reconfiguro la interrupci�n por protecci�n de sobrecorriente
         */
        INTCON3bits.INT1IF = 0; // Borro el flag de INT1
        INTCON3bits.INT1IE = 1; // Habilito la interrupci�n de protecci�n contra sobrecorriente
        INTCONbits.TMR0IF = 0;
    }
    return;
}

void EPWM3_engine_SoftStop(void) {
    if (STATUS_Eng_V.status == ACTIVE_ENGINE) { // Si el motor est� en marcha...
        /* Durante el arranque suave deshabilito la interrupci�n de protecci�n contra 
         * sobrecorriente
         */
        INTCON3bits.INT1IE = 0;
        STATUS_Eng_V.status = STOPPING_ENGINE; // motor en proceso de detenci�n
        if (TMR0_request_period() != TMR0_Stop2)
            TMR0_period(TMR0_Stop2);
        else
            TMR0_Reload();
        TMR0_StartTimer();
        do {
            while (!TMR0_HasOverflowOccured());
            INTCONbits.TMR0IF = 0;
            TMR0_Reload();
            EPWM3_engine_config(STATUS_Eng_V.turn, count_EPWM3_SStop++);
        } while (count_EPWM3_SStop <= STOPPED_ENGINE);
        EPWM3_engine_off();
        TMR0_StopTimer();
        /* Reconfiguro la interrupci�n por protecci�n de sobrecorriente
         */
        INTCON3bits.INT1IF = 0; // Borro el flag de INT1
        INTCON3bits.INT1IE = 1; // Habilito la interrupci�n de protecci�n contra sobrecorriente
        INTCONbits.TMR0IF = 0;
    }
    return;
}

void EPWM3_STOP(void) {
    /* Generaci�n del shutdown del motor de elevaci�n por interrupci�n del comparador
     * que mide sobrecorriente. Se pone al duty al 50% para hacer un frenado el�ctrico.
     * Luego se deshabilita el puente L298.
     */
    uint16_t pulse_width_bit;
    pulse_width_bit = STOPPED_ENGINE * interval_ISR; /* 50 de duty, por el intervalo a "freq"
                                                      * de trabajo
                                                      */
    CCPR3L = pulse_width_bit >> 2;
    CCP3CONbits.DC3B = pulse_width_bit;
    __delay_ms(10); // Espera con el motor activo a duty 50%
    ENB_LAT = LOW;
    ECCP3ASbits.CCP3ASE = 1; //Shutdown de motor de azimut
    T4CONbits.TMR4ON = 0; // Apago el Timer 2
    STATUS_Eng_V.status = INACTIVE_ENGINE; // motor detenido
    STATUS_Eng_V.turn = NONE;
    return;
}