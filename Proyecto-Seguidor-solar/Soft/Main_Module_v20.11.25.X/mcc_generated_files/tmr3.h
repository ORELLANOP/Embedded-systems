
#ifndef TMR3_H
#define TMR3_H

/* v20.11.26
 * Esta librería maneja al Timer 3 para generar interrupciones fijas cada "10ms".
 * Solo se lo apaga o enciende, de modo que cuente o no las ticks. Se destina al toggleo
 * de el led de "Errores en el sistema".
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

void TMR3_Initialize(void);

void TMR3_StartTimer(void);

void TMR3_StopTimer(void);

void TMR3_WriteTimer(uint16_t timerVal);

void TMR3_Reload(void);

void TMR3_ISR(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // TMR3_H