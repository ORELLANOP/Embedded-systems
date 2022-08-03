
#include "epwm2.h"

/* v20.11.30
 * Librería para el manejo del módulo PWM (ECCP2) del PIC18F46K22, destinando
 * su diseño al puente L298. 
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Esta librería solo requiere ubicar en el "pin_manager.h y .c", o directamente
 * desde la paleta del MCC un pin llamado ENA (Enable A), el cual es utilizado
 * para habilitar o no el canal físico del módulo L298
 */

uint8_t count_EPWM2_SStart, count_EPWM2_SStop;

void EPWM2_Initialize(uint8_t freq) {
    TRISC |= 0x03; /* Configuro como entradas los pines RC0-RC1, durante el proceso
                    * de SETUP, para evitar señales espurias. 
                    */
    ENA_LAT = LOW;
    ENA_TRIS = INPUT; /* Configuro como entrada el ENA durante el setup para L298 */
    PSTR2CON = 0x13; /* Las salidas de ECCP2_PWM ocurren en el siguiente período al 
                      * primero del Timer. P2A y P2B se encuentran habilitados para 
                      * funcionar como salida PWM.
                      */
    CCPTMRS0bits.C2TSEL = 0b00; /* El módulo de PWM utiliza como base de tiempo a Timer 2 */
    if (freq < FREC_MIN)
        EPWM2_period(FREC_MIN);
    else
        EPWM2_period(freq);
    ECCP2ASbits.CCP2AS = 0; /* Auto-shutdown (auto apagado) deshabilitado para ECCP2 */
    ECCP2ASbits.PSS2AC = 0; /* Pins PxA and PxC Shutdown State Control bits  
                             * 00 = Pin PxA a "cero" en estado shutdown*/
    ECCP2ASbits.PSS2BD = 0; /* Pins PxB and PxD Shutdown State Control bits
                             * 00 = Pin PxB a "cero" en estado shutdown */
    PWM2CONbits.P2RSEN = 0; /* Auto reinicio deshabilitado. En caso de que ocurra un evento
                             * de shutdown, el bit ASE del registro ECCPxAS, debe limpiarse
                             * por software (Restart Manual por firmware)
                             */
    CCP2CONbits.P2M = 0b10; /* Half-Bridge ECCP2 Module, con P2A y P2B moduladas con 
                             * tiempo de banda muerta.
                             */
    CCP2CONbits.CCP2M = 0b1100; /* Half-Bridge ECCP2 Module. 1100 = PWM mode: 
                                 * P2A active-high; P2B active-high 
                                 * if CCPxM<3:2> = 11: (PWM modes) 
                                 */
    EPWM2_engine_config(NONE, 50); /* Se config. con duty 50%, es decir sin giro */

    PWM2CONbits.P2DC = 0x50; /* PWM Delay Count bits (Dead-Band Delay Mode)) 
                              * configurado para 5us de delay 0x50 = 80  
                              * (Esto genera un "td = 5us") 
                              */
    T2CONbits.T2CKPS = 0b11; /* valor del divisor de la frec de entrada 
                              * (Fosc / 4) -> prescaler = 16 
                              */
    TMR2_StopTimer(); /* Apaga el Timer 2 */
    TMR2_WriteTimer(0);
    TRISC &= 0xFC; /* Configuro como salida los pines RC0-RC1, para P2B y P2A */
    ENA_LAT = LOW;
    ENA_TRIS = OUTPUT; /* Configuro como salida el RE2, pin ENA  para L298 */
    EPWM2_outputs_off();
    STATUS_Eng_A.status = INACTIVE_ENGINE;
    STATUS_Eng_A.turn = NONE;
    return;
}

void EPWM2_period(uint8_t freq) {
    static uint8_t freq_lure = 0;
    if (freq_lure != freq) {
        if (freq <= FREC_MIN)
            TMR2_LoadPeriodRegister((FOSC_kHz / (FREC_MIN * 4 * 16)) - 1);
        else
            TMR2_LoadPeriodRegister((FOSC_kHz / (freq * 4 * 16)) - 1);
        freq_lure = freq;
        EPWM2_duty_const(freq);
    }
    return;
}

float EPWM2_duty_const(uint8_t freq) {
    static float interval = 0;
    if (freq != REQUEST_DUTY_CONST)
        interval = (float) FOSC_kHz / (freq * 100 * 16);
    return interval;
}

void EPWM2_engine_config(uint8_t turn, uint8_t duty) {
    float interval = EPWM2_duty_const(REQUEST_DUTY_CONST);
    uint16_t pulse_width_bit;
    if (turn == ANTI_SOLAR || turn == NONE) {
        pulse_width_bit = duty * interval;
        CCPR2L = pulse_width_bit >> 2;
        CCP2CONbits.DC2B = pulse_width_bit;
    } else if (turn == SOLAR) {
        pulse_width_bit = (100 - duty) * interval;
        CCPR2L = pulse_width_bit >> 2;
        CCP2CONbits.DC2B = pulse_width_bit;
    }
    return;
}

void EPWM2_outputs_off(void) {
    /* Shutdown por firmware */
    ECCP2ASbits.CCP2ASE = 1;
    return;
}

void EPWM2_outputs_on(void) {
    ECCP2ASbits.CCP2ASE = 0;
    return;
}

void EPWM2_engine_running(void) {
    EPWM2_outputs_on();
    TMR2_WriteTimer(0);
    TMR2_StartTimer();
    ENA_LAT = HIGH;
    return;
}

void EPWM2_engine_off(void) {
    if (STATUS_Eng_A.status != INACTIVE_ENGINE) {
        ENA_LAT = LOW;
        EPWM2_outputs_off();
        TMR2_StopTimer();
        STATUS_Eng_A.status = INACTIVE_ENGINE; // motor detenido
        STATUS_Eng_A.turn = NONE;
    }
    return;
}

/* Funciones especiales para mover el Motor 2 con suavidad en el arranque 
 * y apara detenerlo con suavidad cuando deba cortar su funcionamiento 
 */

void EPWM2_engine_SoftStart(uint8_t turn, uint8_t dut_max) {
    if (STATUS_Eng_A.status == INACTIVE_ENGINE) { // Si el motor está inactivo    
        STATUS_Eng_A.turn = turn;
        STATUS_Eng_A.status = STARTING_ENGINE; // Motor en proceso de arranque                              
        /* Accionamiento sobre el motor */
        count_EPWM2_SStart = STOPPED_ENGINE;
        EPWM2_engine_config(turn, count_EPWM2_SStart);
        if (TMR0_request_period() != TMR0_Start1)
            TMR0_period(TMR0_Start1);
        else
            TMR0_Reload();
        EPWM2_engine_running();
        TMR0_StartTimer();
        do {
            while (!TMR0_HasOverflowOccured());
            INTCONbits.TMR0IF = 0;
            TMR0_Reload();
            EPWM2_engine_config(turn, --count_EPWM2_SStart);
        } while (count_EPWM2_SStart >= dut_max);
        count_EPWM2_SStop = dut_max;
        STATUS_Eng_A.status = ACTIVE_ENGINE; // motor en marcha
        TMR0_StopTimer();
        INTCONbits.TMR0IF = 0;
    }
    return;
}

void EPWM2_engine_SoftStop(void) {
    if (STATUS_Eng_A.status == ACTIVE_ENGINE) { // Si el motor está en marcha...
        STATUS_Eng_A.status = STOPPING_ENGINE; // motor en proceso de detención
        if (TMR0_request_period() != TMR0_Stop1)
            TMR0_period(TMR0_Stop1);
        else
            TMR0_Reload();
        TMR0_StartTimer();
        do {
            while (!TMR0_HasOverflowOccured());
            INTCONbits.TMR0IF = 0;
            TMR0_Reload();
            EPWM2_engine_config(STATUS_Eng_A.turn, count_EPWM2_SStop++);
        } while (count_EPWM2_SStop <= STOPPED_ENGINE);
        EPWM2_engine_off();
        TMR0_StopTimer();
        INTCONbits.TMR0IF = 0;
    }
    return;
}
