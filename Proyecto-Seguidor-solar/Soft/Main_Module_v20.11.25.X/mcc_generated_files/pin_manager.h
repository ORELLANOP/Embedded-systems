
#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#include <xc.h>

/* v20.11.30
 * Librería para configuración de pines de I/O del PIC18F46K22.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Se añaden definiciones útiles para cada pin de I/O.
 */

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

// get/set ENA aliases
#define ENA_TRIS                 TRISAbits.TRISA5
#define ENA_LAT                  LATAbits.LATA5

// get/set MFP aliases
#define MFP_TRIS                 TRISBbits.TRISB0
#define MFP_LAT                  LATBbits.LATB0

// get/set Comp_EN2 aliases
#define Comp_EN2_TRIS            TRISBbits.TRISB1
#define Comp_EN2_LAT             LATBbits.LATB1

// get/set BUT_MODE aliases
#define BUT_MODE_TRIS            TRISBbits.TRISB2
#define BUT_MODE_LAT             LATBbits.LATB2

// get/set ENB aliases
#define ENB_TRIS                 TRISCbits.TRISC2
#define ENB_LAT                  LATCbits.LATC2

// get/set A_MUX aliases
#define A_MUX_TRIS               TRISDbits.TRISD2
#define A_MUX_LAT                LATDbits.LATD2

// get/set B_MUX aliases
#define B_MUX_TRIS               TRISDbits.TRISD3
#define B_MUX_LAT                LATDbits.LATD3

// get/set LED_ERROR aliases
#define LED_ERROR_TRIS           TRISDbits.TRISD4
#define LED_ERROR_LAT            LATDbits.LATD4
#define LED_ERROR_Toggle()       do { LATDbits.LATD4 = ~PORTDbits.RD4; } while(0)

// get/set LED_STATE aliases
#define LED_STATE_TRIS           TRISDbits.TRISD5
#define LED_STATE_LAT            LATDbits.LATD5
#define LED_STATE_Toggle()       do { LATDbits.LATD5 = ~PORTDbits.RD5; } while(0)

void PIN_MANAGER_Initialize(void);

#endif // PIN_MANAGER_H