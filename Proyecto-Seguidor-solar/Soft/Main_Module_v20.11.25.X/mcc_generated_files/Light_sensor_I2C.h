
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c2_master.h"
#include "device_config.h"

#ifndef LIGHT_SENSOR_I2C_H
#define	LIGHT_SENSOR_I2C_H

/* v20.11.26
 * Librería para el manejo del módulo BH1750 por medio del MSSP2 del PIC18F46K22. 
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Esta librería utiliza definiciones propias de la librería "i2c1_master.h"
 * más algunas propias.
 */

/* Variable que indica el estado de la comunicación */

typedef struct {
    uint8_t I2C_state;
    uint8_t SENS_ERROR;
    uint8_t i2c_count_error;
    uint8_t Data_Read_error;
} I2C_ERROR;

I2C_ERROR i2c_Error;

/* -----------------------------------------------------------------------------
 *                     DEFINICIONES SEGÚN HOJA DE DATOS
 * -----------------------------------------------------------------------------
 */

//Power

#define BH1750_POWER_DOWN 0x00 // No active state
#define BH1750_POWER_ON 0x01 // Waiting for measurement command
#define BH1750_RESET 0x07 // Reset data register value- No accepted in POWER_DOWN mode

//Measurement Mode

#define CONTINUOUS_HIGH_RES_MODE 0x10 // Measurement at 1 Lux resolution.Measurement time is appro X 120ms 
#define CONTINUOUS_HIGH_RES_MODE_2 0x11 // Measurement at 0.5 Lux resolution.Measurement time is appro X 120ms 
#define CONTINUOUS_LOW_RES_MODE 0x13 // Measurement at 4 Lux resolution.Measurement time is appro X 16ms 
#define ONE_TIME_HIGH_RES_MODE 0x20 // Measurement at 1 Lux resolution.Measurement time is appro X 20ms 
#define ONE_TIME_HIGH_RES_MODE_2 0x21 // Measurement at 0.5 Lux resolution.Measurement time is appro X 120ms 
#define ONE_TIME_LOW_RES_MODE 0x23 // Measurement at 4 Lux resolution.Measurement time is appro X 6ms 

// 

//I2C Address

#define BH1750_1_ADDRESS 0x46 // Sensor connected to GND
#define BH1750_2_ADDRESS 0x47 // Sensor connected to VCC

//I2C Read Measurement

#define Read_measurement_result 0x47

/* -----------------------------------------------------------------------------
 *                DEFINICIONES PARA FUNCIONES DE USUARIO 
 * -----------------------------------------------------------------------------
 */

#define LIGHT_CHANNEL_1   1  // Sensor Superior
#define LIGHT_CHANNEL_2   2  // Sensor inferior Oeste
#define LIGHT_CHANNEL_3   3  // Sensor inferior Este
#define LIGHT_CHANNEL_4   4  // Sensor Luz Total

/* Habilitadores de sensor por multiplexado */

#define LIGHT_SENS_1_EN      {LATD = ((PORTD & 0xF3) | (0x00));} 
#define LIGHT_SENS_4_EN      {LATD = ((PORTD & 0xF3) | (0x04));} 
#define LIGHT_SENS_3_EN      {LATD = ((PORTD & 0xF3) | (0x08));} 
#define LIGHT_SENS_2_EN      {LATD = ((PORTD & 0xF3) | (0x0C));} 

/* Etiquetas para errores de sensores */

#define CORECT_SENS       0
#define ERROR_SENS_1      1
#define ERROR_SENS_2      2
#define ERROR_SENS_3      3
#define ERROR_SENS_4      4
#define MAX_ERROR_COUNT   5

#ifdef	__cplusplus
extern "C" {
#endif

    /* -----------------------------------------------------------------------------
     *                    FUNCIONES DE USUARIO PARA EL SENSOR
     * -----------------------------------------------------------------------------
     */

    /* i2C_BH1750_init habilita el sensor deseado mediante el multiplexor
     * y abre el caño I2C, dejándolo listo para la lectura de datos posterior
     */

    void i2c_BH1750_init(void);

    uint8_t i2c_BH1750_open(uint8_t channel);

    uint16_t BH1750_Data_Read(uint8_t Read_meas);

    uint16_t BH1750_measure(uint8_t channel, uint8_t average);

#ifdef	__cplusplus
}
#endif

#endif	/* LIGHT_SENSOR_I2C_H */