
#ifndef TMR1_H
#define TMR1_H

/* v20.11.26
 * Esta librería maneja al Timer 1 para generar interrupciones fijas cada "10ms".
 * Solo se lo apaga o enciende, de modo que cuente o no las ticks. Se destina al toggleo
 * de el led de "Estado del sistema".
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    void TMR1_Initialize(void);

    void TMR1_StartTimer(void);

    void TMR1_StopTimer(void);

    void TMR1_WriteTimer(uint16_t timerVal);

    void TMR1_Reload(void);

    void TMR1_ISR(void);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif // TMR1_H