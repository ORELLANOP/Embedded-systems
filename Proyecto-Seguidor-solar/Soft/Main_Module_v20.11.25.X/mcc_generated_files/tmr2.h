
#ifndef TMR2_H
#define TMR2_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/* v20.11.26
 * Librería para Timer 2. Aquí se definen los parámetros del Timer 2, 
 * de modo de operar como base de temporización del módulo ECCP2 (EPWM2).
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif

    void TMR2_StartTimer(void);

    void TMR2_StopTimer(void);

    void TMR2_WriteTimer(uint8_t timerVal);

    void TMR2_LoadPeriodRegister(uint8_t periodVal);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif // TMR2_H