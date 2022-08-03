
#include "pin_manager.h"

/* v20.11.30
 * Librería para configuración de pines de I/O del PIC18F46K22.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Se añaden definiciones útiles para cada pin de I/O.
 */

void PIN_MANAGER_Initialize(void) {
    /* Recomendación del fabricante
     */
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    TRISA = 0xE7; /* Todo como entrada menos RA3 y RA4 que quedan como salidas por no ser
                   * utilizadas. RA5 luego es manipulado por ECCP2. RA6 y RA7 dan igual porque
                   * son utilizadas por el cristal y no dependen de lo que se coloque en 
                   * TRISA.
                   */
    TRISB = 0xC7; /* Todo como entrada menos RB3, RB4 y RB5 que no son utilizadas en el proyecto.
                   */
    TRISC = 0xC7; /* Todo como entrada menos RC3, RC4 y RC5 que se ponen como salidas por no ser
                   * utilizadas. RC2 es luego manipulada por ECCP3, así como RC0 y RC1 por ECCP2.
                   */
    TRISD = 0x02; /* Salvo RD6 y RD7, los demás todos se usan. RD0 y RD1 son manipulados por el
                   * MSSP2. RD2 y RD3 son salidas de control al MUX de los sensores de luz.
                   * RD4 y RD5 son salidas para LED de señalización.
                   */
    TRISE = 0x03; /* RE0 y RE1 como entradas (son luego manipuladas por ECCP3)
                   * RE2 no se usa así que se coloca como salida.
                   */

    ANSELA = 0x07; /* RA0, RA1 y RA2 como analógicos 
                    */
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;

    WPUB = 0x04; // Se habilita RB2, donde está el pulsador de cambio de modo
    INTCON2bits.nRBPU = 0; // Pull ups habilitadas
    return;
}