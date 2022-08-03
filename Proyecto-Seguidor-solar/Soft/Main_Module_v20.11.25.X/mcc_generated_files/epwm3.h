
#ifndef EPWM3_H
#define EPWM3_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "tmr4.h"
#include "tmr0.h"
#include "pin_manager.h"
#include "device_config.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.11.30
     * Librería para el manejo del módulo PWM (ECCP3) del PIC18F46K22, destinando
     * su diseño al puente L298. 
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     * Esta librería solo requiere ubicar en el "pin_manager.h y .c", o directamente
     * desde la paleta del MCC un pin llamado ENB (Enable B), el cual es utilizado
     * para habilitar o no el canal físico del módulo L298
     */

    /* Variables y estructuras */

    typedef struct {
        uint8_t status; // Byte indicador del estado del motor
        uint8_t turn; /* Byte indicador del giro actual del motor, o 
                       * o del último sentido en que giró
                       */
    } EPWM3_status;

    EPWM3_status STATUS_Eng_V;

    /* -------------------------------------------------------------------------
     *                   DEFINICIÓN DE CONSTANTES
     * -------------------------------------------------------------------------
     */

#define FOSC_kHz        (_XTAL_FREQ / 1000)
#define FREC_MIN                4            // frecuencia mínima seleccionable

#define NONE                 0x00            // sin sentido de giro (para duty al 50%)  
#define CONTRACT             0x01            // giro a la derecha
#define TO_EXTEND            0x02            // giro a la izquierda

#define REQUEST_DUTY_CONST      0     

    /* -------------------------------------------------------------------------
     *         DEFINICIONES PARA FUNCIONES APLICADAS EN MOTORES
     * -------------------------------------------------------------------------
     */

#define INACTIVE_ENGINE         0    // Motor detenido
#define ACTIVE_ENGINE           1    // Motor en marcha
#define STARTING_ENGINE         2    // Motor arrancando
#define STOPPING_ENGINE         3    // Motor deteniéndose
#define STOPPED_ENGINE         50    // Duty para motor detenido

#define TMR0_Start2            25  // Período para aumento de cada 1% de duty en motor 1
#define TMR0_Stop2             25  // Período para decremento de cada 1% de duty en motor 1

    /* EPWM3_Initialize en base al argumento solicitado, configura el ECCP3 a la 
     * frecuencia especificada, que debe ser mayor o igual a 4kHz y menor o igual
     * a 40kHz. El valor pasado debe estar expresado en kHz. Por ejemplo si se
     * desea 5kHz paso: 5.
     * En la inicialización el módulo PWM y el C.I. L298 inician apagados con duty a 50%.
     */

    void EPWM3_Initialize(uint8_t freq);

    /* EPWM3_period es una función destinada a configurar el período
     * de la señal a la que se le modulará el ancho posteriormente.
     */

    void EPWM3_period(uint8_t freq);

    /* La función EPWM3_duty_const es utilizada exclusivamente por "EPWM3_period"
     * y por "EPWM3_engine_config". Básicamente genera el valor del intervalo que debe
     * haber entre cada % de duty, según la frecuencia de trabajo.
     * La segunda función mencionada realiza una lectura de dicho valor calculado
     * de intervalo, para así poder cargar los registros de configuración de duty 
     * según se solicite.
     */

    float EPWM3_duty_const(uint8_t freq);

    /* EPWM3_engine_config es una función diseñada para el manejo de motores por medio
     * del C.I. L298. Ello implica que configura según el sentido de giro deseado
     * y el duty, la configuración de los registros para que al iniciarse en futuro
     * la operación, se efectue con dichas características.
     *
     * "duty" se especifica en número porcentual, entre aprox. "5" y "50" sin el signo "%".
     * Si se desea en realidad un duty mayor o menor a 50% (esto sobre un motor cambia el 
     * sentido de giro), ello se genera pasandole a la función en el argumento "turn" la 
     * palabra CONTRACT o TO_EXTEND. Si se desea duty al 50% puro, se especifica dicho valor 
     * y en "turn" NONE. 
     */

    void EPWM3_engine_config(uint8_t turn, uint8_t duty);

    /* EPWM3_outputs_off deshabilita físicamente al módulo ECCP3, 
     * generando una operación de shutdown
     */

    void EPWM3_outputs_off(void);

    /* EPWM3_outputs_on, habilita físicamente al módulo ECCP3 */

    void EPWM3_outputs_on(void);

    /* Motor corriendo a las características configuradas */

    void EPWM3_engine_running(void);

    /* Motor detenido */

    void EPWM3_engine_off(void);

    /* EPWM3_SoftStart es una función de arranque suave, partiendo de duty 50%
     * a duty máximo hacia izquiera o derecha, según se haya configurado
     */

    void EPWM3_engine_SoftStart(uint8_t turn, uint8_t dut_max);

    /* EPWM3_SoftStop genera lo contrario a EPWM3_SoftStart, es decir un reposo
     * suave.
     */

    void EPWM3_engine_SoftStop(void);

    /* Rutina de interrupción para detención de EPWM2, en caso de sobrecorriente */

    void EPWM3_STOP(void);

#ifdef __cplusplus  // Provide C++ Compatibility
}

#endif

#endif	//EPWM3_H