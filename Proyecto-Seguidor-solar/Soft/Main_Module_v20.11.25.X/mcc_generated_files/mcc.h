
#ifndef MCC_H
#define	MCC_H

/* Librería v20.11.26
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Contiene las configuraciónes del oscilador y la configuración de los modos de manejo
 * de consumo. También contiene el llamado a todas las funciones de 
 * inicialización del sistema.
 */

#include <xc.h>
#include "device_config.h"
#include "interrupt_manager.h"
#include "i2c2_master.h"
#include "epwm2.h"
#include "memory.h"
#include "adc.h"
#include "epwm3.h"
#include "eusart1.h"
#include "tmr0.h"
//#include "RTCC.h"
#include "Light_sensor_I2C.h"

void SYSTEM_Initialize(void);

void OSCILLATOR_Initialize(void);

#endif	/* MCC_H */