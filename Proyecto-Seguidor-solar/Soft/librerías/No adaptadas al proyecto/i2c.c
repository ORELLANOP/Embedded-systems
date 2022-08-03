#include <xc.h>
#include "i2c.h"

/* v15.5.17i
 * Libreria para el manejo del modulo de comunicacion serie MSSP en modo I2C
 * maestro.
 */

void i2c_init(void){
    SSP1STAT = 0x80;        // SMP      = 1     -> Control del Slew Rate desabilitado para velocidad estandar
                            // CKE      = 0     -> Especificaciones de entrada para SMbus deshabilitado
    SSP1CON1 = 0x28;        // WCOL     = 0     -> Borro bit de deteccion de colisiones
                            //
                            //
                            //
    SSP1CON3 = 0x00;
    SSP1ADD = 0x9F;
    TRISC |= 0x18;
    ANSELC &= 0xE7;
    return;
}

void i2c_gets(char slaveAddress, char address, char *ptrData, char length){
    i2cIdle;
    i2cStart;
    SSP1BUF = slaveAddress;
    i2cIdle;
    i2cAckOk;
    SSP1BUF = address;
    i2cIdle;
    i2cAckOk;
    i2cRestart;
    SSP1BUF = slaveAddress | 0x01;
    i2cIdle;
    i2cAckOk;
    do{
        SSP1CON2bits.RCEN = 1;
        while(SSP1CON2bits.RCEN);
        *ptrData++ = SSP1BUF;
        i2cIdle;
        if(length == 1)
            SSP1CON2bits.ACKDT = 1;
        else
            SSP1CON2bits.ACKDT = 0;
        SSP1CON2bits.ACKEN = 1;
        while(SSP1CON2bits.ACKEN);
    }while(--length != 0);
    i2cStop;
    return;
}

void i2c_puts(char slaveAddress, char address, char *ptrData, char length){
    i2cIdle;
    i2cStart;
    SSP1BUF = slaveAddress;
    i2cIdle;
    i2cAckOk;
    SSP1BUF = address;
    i2cIdle;
    i2cAckOk;
    do{
        SSP1BUF = *(ptrData++);
        i2cIdle;
        i2cAckOk;
    }while(--length != 0);
    i2cStop;
    return;
}

/*
 *
 */
