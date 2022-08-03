
#include "mcc.h"

/* Librería v20.11.26
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Contiene las configuraciónes del oscilador y la configuración de los modos de manejo
 * de consumo. También contiene el llamado a todas las funciones de 
 * inicialización del sistema.
 */

void SYSTEM_Initialize(void) {
    OSCILLATOR_Initialize();
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    EPWM2_Initialize(10); //Frecuencia en kHz    
    EPWM3_Initialize(10); //Frecuencia en kHz  
    ADC_Initialize();
    //RTCC_init();
    TMR0_Initialize(25); //Se inicializa con un período de 25ms
    /* Falta inicializar el Timer 1, 3 y 5 
     */
    i2c2_init();
    i2c_BH1750_init();    
    EUSART1_Initialize();
    return;
}

void OSCILLATOR_Initialize(void) {
    /* Se configura al oscilador principal para Cristal HS (4-16MHz), sin posibilidad
     * de conmutar con el oscilador interno
     */
    OSCCON = 0x08; /* bit 7 -> IDLEN = 0 (El dispositivo entra en modo SLEEP con inst SLEEP)
                    * bit 6-4 -> IRCF = 000 (da igual en realidad ya que no se usa jamás el oscilador interno)
                    * bit 3 -> OSTS = 1 (el dispositivo corre desde el oscilador primario)
                    * bit 2 -> HFIOSF = 0 (no interesa si es o no estable el Oscilador interno en HF)
                    * bit 1-0 -> SCS = 00 (Primary clock (determined by FOSC<3:0> in CONFIG1H))
                    */
    OSCCON2 = 0x04; /* bit 7 -> PLLRDY = 0 (se setea a uno solo cuando esté listo el PLL)
                     * bit 6 -> SOSCRUN = 0 (clock secundario deshabilitado)
                     * bit 5 -> No implementado
                     * bit 4 -> MFIOSEL = 0 (No se utilizan Frecuencias medias del oscilador interno)
                     * bit 3 -> SOSCGO = 0 (El oscilador secundario se apaga si ninguna fuente lo solicita)
                     * bit 2 -> PRISD = 1 (circuito de oscilador primario activado)
                     * bit 1 -> MFIOFS = 0 (MFINTOSC no es estable...da igual este bit en esta config)
                     * bit 0 -> LFIOFS = 0 (idem bit 1 pero para bajas frecuencias en el oscilador interno)
                     */
    while (OSCCON2bits.PLLRDY == 0); /* No sale del bucle hasta que el oscilador 
                                     * activo sea el seleccionado por FOSC con el 4xPLL
                                     */
    return;
}