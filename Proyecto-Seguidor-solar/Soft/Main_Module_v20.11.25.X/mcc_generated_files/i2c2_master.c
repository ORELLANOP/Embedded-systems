
#include "i2c2_master.h"

/* v20.11.27
 * Librería creada para manejar el módulo MSSP2 en modo I2C2 Maestro.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Cabe aclarar que la forma en que esta librería está desarrollada, no contempla
 * el uso de interrupciones. 
 */

void i2c2_init(void) {
    SSP2STAT = 0x80; /* SMP = 1 -> Control del Slew Rate desabilitado para velocidad estandar
                      * CKE = 0 -> Especificaciones de entrada para SMbus deshabilitado
                      */
    SSP2CON1 = 0x28; // WCOL = 0 -> Borro bit de deteccion de colisiones                            
    SSP2CON3 = 0x00;
    SSP2ADD = 0x9F;
    /* Configuración de los pines para el MSSP2, SCL2 (RD0) y SDA2 (RD1) */
    ANSELD &= 0xFC;
    TRISD |= 0x03;
    return;
}

uint8_t i2c2_Open(uint8_t slaveAddress, uint8_t address) {
    if (i2c2_control(_IDLE) == false) {
        i2c2_init(); // Se reinicia la configuración del bus i2c        
        return OPENING_ERROR;
    }
    i2c2Start
    SSP2BUF = slaveAddress;
    if (i2c2_control(_IDLE) == false) {
        i2c2_init();
        return OPENING_ERROR;
    }
    if (i2c2_control(_ACKOK) == false) {
        i2c2_init();
        return OPENING_ERROR;
    }
    SSP2BUF = address;
    if (i2c2_control(_IDLE) == false) {
        i2c2_init();
        return OPENING_ERROR;
    }
    if (i2c2_control(_ACKOK) == false) {
        i2c2_init();
        return OPENING_ERROR;
    }
    return OPENING_SUCCESSFUL;
}

uint8_t i2c2_gets(uint8_t slaveAddress, uint8_t *ptrData, uint8_t length) {
    i2c2Restart
    SSP2BUF = slaveAddress | 0x01; /* El LSB se fuerza a "uno" para indicar operación de 
                                    * lectura.
                                    */
    if (i2c2_control(_IDLE) == false) {
        i2c2_init();
        return GET_ERROR;
    }
    if (i2c2_control(_ACKOK) == false) {
        i2c2_init();
        return GET_ERROR;
    }
    do { /* Obtención de los datos 
          */
        SSP2CON2bits.RCEN = 1;
        if (i2c2_control(_RCEN) == false) {
            i2c2_init();
            return GET_ERROR;
        }
        *ptrData++ = SSP1BUF;
        if (i2c2_control(_IDLE) == false) {
            i2c2_init(); // Se reinicia la configuración del bus i2c        
            return GET_ERROR;
        }
        if (length == 1)
            SSP2CON2bits.ACKDT = 1;
        else
            SSP2CON2bits.ACKDT = 0;
        SSP2CON2bits.ACKEN = 1;
        while (SSP2CON2bits.ACKEN);
    } while (--length != 0);
    i2c2Stop
    return GET_SUCCESSFUL;
}

uint8_t i2c2_puts(uint8_t *ptrData, uint8_t length) {
    do {
        SSP2BUF = *(ptrData++);
        if (i2c2_control(_IDLE) == false) {
            i2c2_init(); // Se reinicia la configuración del bus i2c2        
            return PUT_ERROR;
        }
        if (i2c2_control(_ACKOK) == false) {
            i2c2_init(); // Se reinicia la configuración del bus i2c2        
            return PUT_ERROR;
        }
    } while (--length != 0);
    i2c2Stop;
    return PUT_SUCCESSFUL;
}

bool i2c2_control(uint8_t bit_field) {
    if (TMR0_request_period() != TMR0_i2c2_ctrl)
        TMR0_period(TMR0_i2c2_ctrl);
    else
        TMR0_Reload();
    INTCONbits.TMR0IF = 0;
    TMR0_StartTimer();
    do {
        switch (bit_field) {
            case _IDLE:
                if (((SSP2CON2 & 0x1F) || (SSP2STATbits.R_nW)) == 0) {
                    TMR0_StopTimer();
                    INTCONbits.TMR0IF = 0;
                    return true; // El dispositivo respondió correctamente, se devuelve "true"
                }
                break;
            case _ACKOK:
                if (SSP2CON2bits.ACKSTAT == 0) {
                    TMR0_StopTimer();
                    INTCONbits.TMR0IF = 0;
                    return true; // El dispositivo respondió correctamente, se devuelve "true"
                }
                break;
            case _RCEN:
                if (SSP2CON2bits.RCEN == 0) {
                    TMR0_StopTimer();
                    INTCONbits.TMR0IF = 0;
                    return true; // El dispositivo respondió correctamente, se devuelve "true"
                }
                break;
        }
    } while (!TMR0_HasOverflowOccured());
    TMR0_StopTimer();
    INTCONbits.TMR0IF = 0;
    return false;
}