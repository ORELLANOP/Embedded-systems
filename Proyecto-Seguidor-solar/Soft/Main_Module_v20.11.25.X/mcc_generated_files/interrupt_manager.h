
#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include "eusart1.h"
#include "epwm2.h"
#include "epwm3.h"
#include "tmr1.h"
#include "tmr3.h"

/* v20.11.30
 * Librería diseñada para la configuración y manejo de los periféricos y módulos
 * del procesador que trabajan mediante interrupciones. 
 * Las interrupciones externas se configuran en esta librería. Las de los periféricos
 * como la EUSART se configuran en su propia librería.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

#define INTERRUPT_GlobalInterruptHighEnable() (INTCONbits.GIEH = 1)
#define INTERRUPT_GlobalInterruptHighDisable() (INTCONbits.GIEH = 0)

#define INTERRUPT_GlobalInterruptLowEnable() (INTCONbits.GIEL = 1)
#define INTERRUPT_GlobalInterruptLowDisable() (INTCONbits.GIEL = 0)

#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

/* Definiciones para función de cambio de modo por el pulsador */

#define NO_CHANGE   0
#define MANUAL_MOD  1
#define DEMO_MOD    2
#define AUTO_MOD    3

uint8_t MODE_BUTTON;

void INTERRUPT_Initialize (void);

void EXT_INT_init(void);

void INT0_ISR(void);

void INT1_ISR(void);

void INT2_ISR(void);

#endif  // INTERRUPT_MANAGER_H