#include <xc.h>
#include "adc.h"

/* v15.3.19i
 * Libreria para el manejo del conversor analogico - digital interno del
 * microcontrolador.
 * Optimizado para las conexiones de la placa de la interface.
 */

void adc_init(void){
    LATA = 0x00;        // Borrar los latch del puerto A
    TRISA = (TRISA | 0x0B);
    ANSELA = 0x0B;      // AN0, AN1 y AN3 pines analogicos
    VREFCON0 = 0xB0;    // FVREN      = 1   -> Voltaje de referencia fijo habilitado
                        // FVRS<1:0>  = 11  -> Voltaje a 4.096V
    ADCON0 = 0x01;      // ADON       = 1   -> ADC Habilitado
    ADCON1 = 0x08;      // PVCFG<1:0> = 10  -> Vref+ conectado a la señal FVR BUF2
                        // NVCFG<1:0> = 00  -> Vref- conectado a AVss
    ADCON2 = 0x8E;      // ADFM       = 0   -> Resultado justificado a la derecha
                        // ACQT<2:0>  = 001 -> 2 TDA
                        // ADCS<2:0>  = 110 -> Fosc/64
    ADRESL = 0x00;      // Borro ADRESL
    ADRESH = 0x00;      // Borro ADRESH
    return;
}

unsigned int adc_getConv(unsigned int channel){
    ADCON0bits.CHS = channel;           // Asigno el canal a leer
    ADCON0bits.GO_nDONE = 1;            // Inico la conversion
    while(ADCON0bits.GO_nDONE);         // Espero a que la conversion termine
    return ((ADRESH << 8) + ADRESL);    // Devuelvo el valor leido
}

/*
 *
 */
