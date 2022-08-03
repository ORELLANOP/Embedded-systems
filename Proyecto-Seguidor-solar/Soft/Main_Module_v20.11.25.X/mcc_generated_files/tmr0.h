
#ifndef TMR0_H
#define TMR0_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "device_config.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.09.18
     * Librer�a para Timer 0. Este Temporizador se utiliza para generar los arranques suaves y 
     * detenciones suaves de los motores. Para ello el timer puede configurarse entre 5ms
     * y 131ms (prescaler en 1:32), con el fin de que en cada paso de dicho tiempo, 
     * se aumente o disminuta seg�n corresponda, un "1%" en el duty del motor que se est� operando. 
     * Tambi�n se utiliza esta librer�a como "whach dog" del i2c2 en caso de no respuesta.
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     */

    /* ---------------------------------------------------------------------
     *               DEFINICIONES PARA Timer 0
     * ---------------------------------------------------------------------
     */

#define FOSC_kHz (_XTAL_FREQ / 1000)
#define UINT16_MAXVALUE 65535
#define PERIOD_MIN 5
#define PERIOD_MAX 131

    /* Funci�n que congigura e inicializa al Timer*/

    void TMR0_Initialize(uint8_t period);

    /* Funci�n para cambiar el Per�odo del Timer (ms) en tiempo de ejecuci�n */

    void TMR0_period(uint8_t per);

    /* Funci�n que devuelve el valor actual de per�odo seleccionado en "milisegundos" */

    uint8_t TMR0_request_period(void);

    void TMR0_StartTimer(void);

    void TMR0_StopTimer(void);

    uint16_t TMR0_WriteTimer(uint8_t timerVal);

    void TMR0_Reload(void);

    bool TMR0_HasOverflowOccured(void);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif // TMR0_H