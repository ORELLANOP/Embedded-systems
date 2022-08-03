
#include "adc.h"

/* v20.11.26
 * Librería para el manejo del módulo ADC del PIC18F46K22.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Se destina su uso a la conversión de tensiones provenientes de diferentes 
 * sensores: posición azimutal tipo potenciómetro, tensión de panel, corriente en la 
 * carga por medio de sensor de efecto Hall "Allegro".
 */

void ADC_Initialize(void) {
    TRISA |= 0x07; // RA0, RA1 y RA2 como entradas...
    ANSELA |= 0x07; // analógicas.
    ADCON0 = 0x05; // GO_nDONE stop; ADON enabled; CHS AN1;       
    ADCON1 = 0x00; // TRIGSEL CCP5; NVCFG VSS; PVCFG VDD;  
    ADCON2 = 0x96; // ADFM right; ACQT 4 TAD; ADCS FOSC/64; -> 1 TAD = 64/XTAL = 1us
    ADRESL = 0x00;
    ADRESH = 0x00;
    return;
}

uint16_t ADC_GetConversion(uint8_t channel, uint8_t averaged) {
    uint16_t accum = 0;
    uint8_t count = 0;
    ADCON0bits.CHS = channel; // selecciona el canal a convertir
    do {
        ADCON0bits.GO_nDONE = true;
        /* Espera 1 ACQT (4 TAD) para la adquisición del sample and hold.
         * Luego 11 TAD para convertir....
         */
        while (ADCON0bits.GO_nDONE);
        accum += ((ADRESH << 8) + ADRESL);
        count++;
        __delay_us(2); //Espero 2TAD antes de al próxima adquisición
        /* En total desde ADC_StartConvesion pasan aproximadamente 17 TAD (17us)
         */
    } while (count < averaged);
    return (accum / averaged);
}