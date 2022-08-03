
#ifndef TMR4_H
#define TMR4_H

/* v20.11.26
 * Librería para Timer 4. Aquí se definen los parámetros del Timer 4, 
 * de modo de operar como base de temporización del módulo ECCP3 (EPWM3).
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    void TMR4_StartTimer(void);

    void TMR4_StopTimer(void);

    void TMR4_WriteTimer(uint8_t timerVal);

    void TMR4_LoadPeriodRegister(uint8_t periodVal);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif // TMR4_H