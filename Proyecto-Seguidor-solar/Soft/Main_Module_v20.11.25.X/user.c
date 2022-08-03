
#include "user.h"

/* v20.11.13
 * 
 * En esta librería se encuentran todas las funciones de usuario que se encargan de hacer
 * uso del resto de las librerías, resolviendo tareas específicas al proyecto.
 * Esto no implica que en algunas librerías como la del manejo de motores (epwm2 y epwm3)
 * no haya funciones específicas también.
 */

/* Arreglos y variables para operar las lecturas de la usart */

uint8_t command[COMMAND_BUFFER]; // Arreglo para almacenar comando leido de la usart
uint8_t caract_count; // contador de caracteres leidos antes del "\0"

/* Variables para controlar el funcionamiento del i2c */

uint8_t i2c_error_counter = 0;

/* -----------------------------------------------------------------------------
 *                              SECCIÓN DE FUNCIONES
 * -----------------------------------------------------------------------------
 */

void init_App(void) {
    uint8_t i = 0;
    /* Inicialización de todos los elementos del arreglo de comando, en "NULL" */
    for (i = 0; i < COMMAND_BUFFER; i++)
        command[i] = '\0';
    caract_count = 0;
    /* Inicialización de parámetros del sistema */
    syst_Stat.i2c_stat = CORRECT;
    request_pos.azimut_angle = ADC_GetConversion(ANG_AZIMUT, 10); // init, angle measure
    /* Verificación de antemano, para saber si está o no en un limite el sens. pos azimuth
     */
    if (request_pos.azimut_angle <= MAX_SOLAR)
        syst_Stat.lim_azimuth = LIM_SOLAR;
    else if (request_pos.azimut_angle >= MAX_ANTISOLAR)
        syst_Stat.lim_azimuth = LIM_ANTISOLAR;
    else
        syst_Stat.lim_azimuth = NOT_ARRIVE;

    request_pos.elevation_angle = 0;
    previous_pos.azimut_angle = request_pos.azimut_angle;
    return;
}

/* Funciones de lectura de comandos recibidos por EUSART */

void command_get(void) {
    uint8_t i = 0;
    do {
        command[i++] = getche();
        caract_count++;
    } while (command[i - 1] != '\0');
    caract_count--;
    return;
}

uint8_t command_execution(void) {
    if (EUSART1_is_rx_ready()) { // Si el Buffer de recepción tiene algo pendiente por ser leido..
        caract_count = 0;
        command_get();
        if (caract_count == 1) {
            return short_commands(command[SHORT_COMMAND]);
        }
    }
    return NO_ACTION;
}

uint8_t short_commands(uint8_t short_comm) {
    uint8_t azi_ctrl;
    /* EJECUCIÓN DE COMANDOS CORTOS POR SWITCH CASE */
    switch (short_comm) {
        case ANTISOLAR_MOVE: // Movimiento Anti-Solar (este)
            if (syst_Stat.mode == MANUAL_MODE && syst_Stat.lim_azimuth == NOT_ARRIVE) {
                azi_ctrl = azi_lim_control(ANTI_SOLAR); // Control de sensor de posición Azimut
                if (azi_ctrl == EXIT_LIMIT_AZIMUT || azi_ctrl == UNEXPECTED_JUMP_AZI)
                    return azi_ctrl;
                else {
                    if (STATUS_Eng_A.status == INACTIVE_ENGINE)
                        printf("$e;");
                    EPWM2_engine_SoftStart(ANTI_SOLAR, 10);
                    if (syst_Stat.lim_azimuth == LIM_SOLAR)
                        syst_Stat.lim_azimuth = NOT_ARRIVE;
                    return ANTISOLAR_MOVE;
                }
            }
            break;
        case SOLAR_MOVE: // Movimiento Solar (oeste)
            if (syst_Stat.mode == MANUAL_MODE && syst_Stat.lim_azimuth == NOT_ARRIVE) {
                azi_ctrl = azi_lim_control(SOLAR); // Control de sensor de posición Azimut
                if (azi_ctrl == EXIT_LIMIT_AZIMUT || azi_ctrl == UNEXPECTED_JUMP_AZI)
                    return azi_ctrl;
                else {
                    if (STATUS_Eng_A.status == INACTIVE_ENGINE)
                        printf("$o;");
                    EPWM2_engine_SoftStart(SOLAR, 10);
                    if (syst_Stat.lim_azimuth == LIM_ANTISOLAR)
                        syst_Stat.lim_azimuth = NOT_ARRIVE;
                    return SOLAR_MOVE;
                }
            }
            break;
        case TO_EXTEND_MOVE: // Movimiento "Extensor" (subir)
            if (syst_Stat.mode == MANUAL_MODE) {
                if (STATUS_Eng_V.status == INACTIVE_ENGINE)
                    printf("$s;");
                EPWM3_engine_SoftStart(TO_EXTEND, 5);
                return TO_EXTEND_MOVE;
            }
            break;
        case CONTRACT_MOVE: // Movimiento "Contractor" (bajar)
            if (syst_Stat.mode == MANUAL_MODE) {
                if (STATUS_Eng_V.status == INACTIVE_ENGINE)
                    printf("$b;");
                EPWM3_engine_SoftStart(CONTRACT, 5);
                return CONTRACT_MOVE;
            }
            break;
        case STOP_ENGINES: // parar ambos motores por orden WIFI       
            flag_jump_azi_pos = false; //Prueba
            previous_pos.azimut_angle = current_pos.azimut_angle;
            if (Emergency_stop() == EXIT_EMERGENCY_STOP)
                return EXIT_EMERGENCY_STOP;
            break;
            /* MODOS DE FUNCIONAMIENTO, seleccionables desde la APP */
        case AUTONOMOUS_MODE: // modo de funcionamiento autónomo
            if (syst_Stat.mode != AUTONOMOUS_MODE) {
                syst_Stat.mode = AUTONOMOUS_MODE;
                DATAEE_WriteByte(STATUS_SYSTEM, syst_Stat.mode);
                printf("$a;");
                return EXIT_MODE_CHANGE;
            }
            break;
        case DEMO_MODE: // modo de funcionamiento demo
            if (syst_Stat.mode != DEMO_MODE) {
                syst_Stat.mode = DEMO_MODE;
                DATAEE_WriteByte(STATUS_SYSTEM, syst_Stat.mode);
                printf("$d;");
                return EXIT_MODE_CHANGE;
            }
            break;
        case MANUAL_MODE: // modo de funcionamiento manual
            if (syst_Stat.mode != MANUAL_MODE) {
                syst_Stat.mode = MANUAL_MODE;
                DATAEE_WriteByte(STATUS_SYSTEM, syst_Stat.mode);
                printf("$m;");
                return EXIT_MODE_CHANGE;
            }
            break;        
        case CHECK_MODE: // Consulta de modo
            mode_check();
            break;
        case CHECK_AZIMUT: // Pedido de ángulo azimut
            
            return CHECK_AZIMUT;
        
        case STATE_TOTAL_LIGHT: // Pedido sensor de luz Total
            
            break;
        case STATE_CARDINAL_SENSORS: // Pedido de los 3 sensores cardinales
            
            break;
        case STATE_I_PANEL: // Pedido de corriente del panel
            
            break;
        
        case PLACE_HORIZONTALLY:
            
            break;
    }
    return NO_ACTION;
}

void mode_check(void) {
    switch (syst_Stat.mode) {
        case AUTONOMOUS_MODE:
            printf("$a;");
            break;
        case DEMO_MODE:
            printf("$d;");
            break;
        case MANUAL_MODE:
            printf("$m;");
            break;
    }
    return;
}

uint8_t Emergency_stop(void) {
    if ((STATUS_Eng_A.status == ACTIVE_ENGINE) || (STATUS_Eng_V.status == ACTIVE_ENGINE)) {
        EPWM2_engine_SoftStop();
        EPWM3_engine_SoftStop();
        printf("$P;");
        if (syst_Stat.mode != MANUAL_MODE) {
            syst_Stat.mode = MANUAL_MODE;
            DATAEE_WriteByte(STATUS_SYSTEM, syst_Stat.mode);
            printf("$m;");
        }
        return EXIT_EMERGENCY_STOP;
    }
    return NO_ACTION;
}

/* -----------------------------------------------------------------------------
 *                      FUNCIONES DE MANEJO DE ADC
 * -----------------------------------------------------------------------------
 */

uint8_t jump_azi_ctrl(void) {
    uint16_t dif_value;
    if (flag_jump_azi_pos == false) {
        dif_value = abs(current_pos.azimut_angle - previous_pos.azimut_angle);
        previous_pos.azimut_angle = current_pos.azimut_angle;
        if (dif_value > HYST_ADC_POS) {
            flag_jump_azi_pos = true;
            if (STATUS_Eng_A.status == ACTIVE_ENGINE) {
                EPWM2_engine_SoftStop();
                printf("$F2;"); // Parado por error en el sensor de posición (error)
            }
            return UNEXPECTED_JUMP_AZI;
        }
    } else
        return UNEXPECTED_JUMP_AZI;
    return CORRECT_EXECUTION;
}

uint8_t azi_lim_control(uint8_t move) {
    current_pos.azimut_angle = ADC_GetConversion(ANG_AZIMUT, 10);
    switch (move) {
        case SOLAR:
            if (jump_azi_ctrl() == UNEXPECTED_JUMP_AZI) {
                return UNEXPECTED_JUMP_AZI;
            }
            if (current_pos.azimut_angle < MAX_SOLAR) {
                if (STATUS_Eng_A.status == ACTIVE_ENGINE) {
                    EPWM2_engine_SoftStop();
                    printf("$f;"); // Parado por final de carrera
                }
                if (syst_Stat.lim_azimuth == NOT_ARRIVE)
                    syst_Stat.lim_azimuth = LIM_SOLAR;
                return EXIT_LIMIT_AZIMUT;
            }
            break;
        case ANTI_SOLAR:
            if (jump_azi_ctrl() == UNEXPECTED_JUMP_AZI)
                return UNEXPECTED_JUMP_AZI;
            if (current_pos.azimut_angle > MAX_ANTISOLAR) {
                if (STATUS_Eng_A.status == ACTIVE_ENGINE) {
                    EPWM2_engine_SoftStop();
                    printf("$f;"); // Parado por final de carrera
                }
                if (syst_Stat.lim_azimuth == NOT_ARRIVE)
                    syst_Stat.lim_azimuth = LIM_ANTISOLAR;
                return EXIT_LIMIT_AZIMUT;
            }
            break;
    }
    return CORRECT_EXECUTION;
}

int16_t get_current(uint8_t chan) {
    return 0;
}

/* -----------------------------------------------------------------------------
 *                   FUNCIONES DE CONTROL AVANZADO DE MOTORES 
 * -----------------------------------------------------------------------------
 */

uint8_t azimuth_correction(uint8_t pos) {
   
    return NO_ACTION;
}

uint8_t azi_motion_ctrl(uint8_t move, uint8_t chann_A, uint8_t chann_B) {
    
    return CORRECT_EXECUTION;
}

uint8_t elev_correction(uint8_t pos) {
    
    return NO_ACTION;
}

uint8_t elev_motion_ctrl(uint8_t chann_A, uint8_t chann_B) {
    
    return CORRECT_EXECUTION;
}