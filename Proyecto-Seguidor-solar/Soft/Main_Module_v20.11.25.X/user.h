#ifndef USER_H
#define	USER_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "mcc_generated_files/adc.h"
#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/epwm2.h"
#include "mcc_generated_files/epwm3.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr0.h"
#include "mcc_generated_files/Light_sensor_I2C.h"
#include "mcc_generated_files/memory.h"

#ifdef	__cplusplus
extern "C" {
#endif  

    /* v20.11.13
     * 
     * En esta librería se encuentran todas las funciones de usuario que se encargan de hacer
     * uso del resto de las librerías, resolviendo tareas específicas al proyecto.
     * Esto no implica que en algunas librerías como la del manejo de motores (epwm2 y epwm3)
     * no haya funciones específicas también.
     */

    /* Estructuras y variables */
    
    bool flag_jump_azi_pos;
    
    typedef struct {
        uint8_t mode;
        uint8_t i2c_stat;
        uint8_t lim_azimuth;
    } STATUS_SYST;

    typedef struct {        
        int16_t I_panel; // Corriente sensada de modo indirecto por el sensor de efecto Hall
        float V_panel; // Tensión del panel
        float P_panel; // Potencia consumida en el panel
    } Electrical_measurements;

    typedef struct {
        uint16_t Average_Sensor[3]; // sensores cardinales
        uint16_t Average_Sensor_Total; // sensor independiente, paralelo al panel
    } Sampling_light;

    typedef struct {
        uint16_t azimut_angle; // Ángulo de azimut
        uint16_t elevation_angle; // Ángulo de elevación
    } Position_parameters;

    Electrical_measurements request_elec;
    Sampling_light request_light;
    Position_parameters current_pos, previous_pos, request_pos;
    STATUS_SYST syst_Stat;

    /* -------------------------------------------------------------------------
     *              DEFINICIONES DEL ESTADO DEL SISTEMA EN GENERAL
     * -------------------------------------------------------------------------
     */

#define NO_ACTION                 0 // No se ejecuta nada dentro de alguna función
#define CORRECT_EXECUTION         1 // Ejecución correcta en alguna función
#define EXIT_MODE_CHANGE          2 // Salida por cambio de modo
#define EXIT_EMERGENCY_STOP       3 // Salida por detención manual de motores
#define EXIT_LIMIT_AZIMUT         4 // Salida por límite en azimut
#define ORIENTED_SYSTEM           6 // sistema oriendato
#define FIXED_IN_AZIMUT           7 // Sistema corregido en azimut
#define FIXED_IN_ELEV             8 // Sistema corregido en elevación
#define UNEXPECTED_JUMP_AZI       9 // Salto inesperado en sensor de posición azimuth
#define EXIT_INSUFF_LIGHT        10 // Salida por nivel de luz insuficiente

    /* -------------------------------------------------------------------------
     *           COMANDOS CORTOS DE COMUNICACIÓN CON MÓDULO WIFI
     * -------------------------------------------------------------------------
     */

#define ANTISOLAR_MOVE           'e'
#define SOLAR_MOVE               'o'  // mov a derecha mirando al panel de frente
#define TO_EXTEND_MOVE           's'
#define CONTRACT_MOVE            'b'
#define STOP_ENGINES             'p' 
#define AUTONOMOUS_MODE          'a'
#define DEMO_MODE                'd' 
#define MANUAL_MODE              'm'  
#define CHECK_CONECTIVITY        '?'
#define CHECK_MODE               'M'
#define CHECK_AZIMUT             'A'
#define CHECK_ELEVATION          'E'
#define STATE_TOTAL_LIGHT        'l'
#define STATE_CARDINAL_SENSORS   'L'
#define STATE_I_PANEL            'c'
#define CORRECT_REQ_AUTO_MODE    'x' // Solicitud de corrección en modo autónomo ya activo
#define PLACE_HORIZONTALLY       'h' // Colocar al panel horizontal, solo en modo manual

    /* -------------------------------------------------------------------------
     *                            COMANDOS LARGOS
     * -------------------------------------------------------------------------
     */

    /* Respecto al arreglo en el que se va haciendo lectura de los comandos largos
     * o cortos que se reciben desde el buffer de la EUSART 
     */

#define SHORT_COMMAND             0
#define COMMAND_BUFFER           24

    /* -------------------------------------------------------------------------
     *      Definiciones para uso de los sensores de posición 
     * -------------------------------------------------------------------------
     */

#define MAX_SOLAR             38    /* Medición con el ADC del sensor de posición AZIMUT. 
                                       Máxima posición para sentido SOLAR 
                                     */
#define MAX_ANTISOLAR        736    /* Medición con el ADC del sensor de posición AZIMUT.
                                       Máxima posición para sentido ANTI_SOLAR
                                     */
#define HYST_ADC_POS          50    /* Histéresis entre valor y valor medido por el ADC 
                                     * en el sensor de azimuth
                                     */

#define NOT_ARRIVE             0 // Motor no llegó al límite
#define LIM_SOLAR              1 // Motor azimuth en límite solar
#define LIM_ANTISOLAR          2 // Motor azimuth en límite antisolar

    /* Otros parámetros*/

#define HYSTERESYS           400 /* Margen de histéresis por nivel en 
                                  * Lux para comparación entre sensores
                                  */
#define LIGHT_LEVEL_LIMIT   4000 /* Nivel de luz en LUX, despues del cual los motores
                                  * dejarán de intentar corregir
                                  */


    /* Inicia cuestiones propias de la aplicación, variables y 
     * funciones de usuario 
     */

    void init_App(void);

    /* Función utilizada exclusivamente por la EUSART para almacenar lo recién recibido */

    void command_get(void);

    /* La función command_execution, ejecuta comandos cortos o largos, siempre que la usart esté lista
     * para ofrecer un comando (es decir que haya recibido algo). Por lo tanto solo hace lectura del comendo
     * que se pueda leer en el buffer de recepción de la EUSART.*/

    uint8_t command_execution(void);

    uint8_t short_commands(uint8_t short_comm);

    /* función que envía por usart el modo "actual" de operación del sistema */

    void mode_check(void);

    uint8_t Emergency_stop(void);

    /* -----------------------------------------------------------------------------
     *                      FUNCIONES DE MANEJO DE ADC
     * -----------------------------------------------------------------------------
     */
    
    uint8_t jump_azi_ctrl(void);

    uint8_t azi_lim_control(uint8_t move);

    int16_t get_current(uint8_t chan);

    /* -----------------------------------------------------------------------------
     *                   FUNCIONES DE CONTROL AVANZADO DE MOTORES 
     * -----------------------------------------------------------------------------
     */

    uint8_t azimuth_correction(uint8_t pos);

    uint8_t azi_motion_ctrl(uint8_t move, uint8_t chann_A, uint8_t chann_B);

    uint8_t elev_correction(uint8_t pos);

    uint8_t elev_motion_ctrl(uint8_t chann_A, uint8_t chann_B);


#ifdef	__cplusplus
}
#endif

#endif	/* USER_H */
