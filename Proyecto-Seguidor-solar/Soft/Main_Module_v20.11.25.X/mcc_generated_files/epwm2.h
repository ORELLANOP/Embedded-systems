
#ifndef EPWM2_H
#define EPWM2_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "tmr2.h"
#include "tmr0.h"
#include "pin_manager.h"
#include "device_config.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.11.30
     * Librer�a para el manejo del m�dulo PWM (ECCP2) del PIC18F46K22, destinando
     * su dise�o al puente L298. 
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     * Esta librer�a solo requiere ubicar en el "pin_manager.h y .c", o directamente
     * desde la paleta del MCC un pin llamado ENA (Enable A), el cual es utilizado
     * para habilitar o no el canal f�sico del m�dulo L298
     */

    /* Variables y estructuras */

    typedef struct {
        uint8_t status; // Byte indicador del estado del motor
        uint8_t turn; /* Byte indicador del giro actual del motor, o 
                       * o del �ltimo sentido en que gir�
                       */
    } EPWM2_status;

    EPWM2_status STATUS_Eng_A;

    /* -------------------------------------------------------------------------
     *                   DEFINICI�N DE CONSTANTES
     * -------------------------------------------------------------------------
     */

#define FOSC_kHz        (_XTAL_FREQ / 1000)
#define FREC_MIN                4            // frecuencia m�nima seleccionable

#define NONE                 0x00            // sin sentido de giro (para duty al 50%)
#define ANTI_SOLAR           0x01            // giro a la derecha
#define SOLAR                0x02            // giro a la izquierda

#define REQUEST_DUTY_CONST      0 

    /* -------------------------------------------------------------------------
     *         DEFINICIONES PARA FUNCIONES APLICADAS EN MOTORES
     * -------------------------------------------------------------------------
     */

#define INACTIVE_ENGINE         0  // Motor detenido
#define ACTIVE_ENGINE           1  // Motor en marcha
#define STARTING_ENGINE         2  // Motor arrancando
#define STOPPING_ENGINE         3  // Motor deteni�ndose
#define STOPPED_ENGINE         50  // Duty para motor detenido

#define TMR0_Start1            25  // Per�odo para aumento de cada 1% de duty en motor 1
#define TMR0_Stop1             25  // Per�odo para decremento de cada 1% de duty en motor 1

    /* EPWM2_Initialize en base al argumento solicitado, configura el ECCP2 a la 
     * frecuencia especificada, que debe ser mayor o igual a 4kHz y menor o igual
     * a 40kHz. El valor pasado debe estar expresado en kHz. Por ejemplo si se
     * desea 5kHz paso: 5.
     * En la inicializaci�n el m�dulo PWM y el C.I. L298 inician apagados con duty a 50%.
     */

    void EPWM2_Initialize(uint8_t freq);

    /* EPWM2_period es una funci�n destinada a configurar el per�odo
     * de la se�al a la que se le modular� el ancho posteriormente.
     */

    void EPWM2_period(uint8_t freq);

    /* La funci�n EPWM2_duty_const es utilizada exclusivamente por "EPWM2_period"
     * y por "EPWM2_engine_config". B�sicamente genera el valor del intervalo que debe
     * haber entre cada % de duty, seg�n la frecuencia de trabajo.
     * La segunda funci�n mencionada realiza una lectura de dicho valor calculado
     * de intervalo, para as� poder cargar los registros de configuraci�n de duty 
     * seg�n se solicite.
     */

    float EPWM2_duty_const(uint8_t freq);

    /* EPWM2_engine_config es una funci�n dise�ada para el manejo de motores por medio
     * del C.I. L298. Ello implica que configura seg�n el sentido de giro deseado
     * y el duty, la configuraci�n de los registros para que al iniciarse en futuro
     * la operaci�n, se efectue con dichas caracter�sticas.
     *
     * "duty" se especifica en n�mero porcentual, entre aprox. "5" y "50" sin el signo "%".
     * Si se desea en realidad un duty mayor o menor a 50% (esto sobre un motor cambia el 
     * sentido de giro), ello se genera pasandole a la funci�n en el argumento "turn" la 
     * palabra ANTI_SOLAR o SOLAR. Si se desea duty al 50% puro, se especifica dicho valor 
     * y en "turn" NONE. 
     */

    void EPWM2_engine_config(uint8_t turn, uint8_t duty);

    /* EPWM2_outputs_off, deshabilita f�sicamente al m�dulo ECCP2, 
     * generando una operaci�n de shutdown
     */

    void EPWM2_outputs_off(void);

    /* EPWM2_outputs_on, habilita f�sicamente al m�dulo ECCP2 */

    void EPWM2_outputs_on(void);

    /* Motor corriendo a las caracter�sticas configuradas */

    void EPWM2_engine_running(void);

    /* Motor detenido */

    void EPWM2_engine_off(void);

    /* EPWM2_SoftStart es una funci�n de arranque suave, partiendo de duty 50%
     * a duty m�ximo hacia izquiera o derecha, seg�n se haya configurado
     */

    void EPWM2_engine_SoftStart(uint8_t turn, uint8_t dut_max);

    /* EPWM2_SoftStop genera lo contrario a EPWM2_SoftStart, es decir un reposo
     * suave.
     */

    void EPWM2_engine_SoftStop(void);

#ifdef __cplusplus  // Provide C++ Compatibility
}

#endif

#endif	//EPWM2_H