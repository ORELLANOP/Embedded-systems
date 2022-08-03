
#include "Light_sensor_I2C.h"

/* v20.11.26
 * Librería para el manejo del módulo BH1750 por medio del MSSP2 del PIC18F46K22. 
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Esta librería utiliza definiciones propias de la librería "i2c1_master.h"
 * más algunas propias.
 */

void i2c_BH1750_init(void) {
    i2c_Error.i2c_count_error = 0; // Contador de errores inicia en cero.   
    /* Configuración de pines para el Mux */
    TRISD &= 0xF3; // Configuro RD3 - RD2 como salidas    
    BH1750_measure(LIGHT_CHANNEL_1, 1);
    BH1750_measure(LIGHT_CHANNEL_2, 1);
    BH1750_measure(LIGHT_CHANNEL_3, 1);
    BH1750_measure(LIGHT_CHANNEL_4, 1);
    return;
}

uint8_t i2c_BH1750_open(uint8_t channel) {
    switch (channel) {
        case LIGHT_CHANNEL_1:
            LIGHT_SENS_1_EN
            break;
        case LIGHT_CHANNEL_2:
            LIGHT_SENS_2_EN
            break;
        case LIGHT_CHANNEL_3:
            LIGHT_SENS_3_EN
            break;
        case LIGHT_CHANNEL_4:
            LIGHT_SENS_4_EN
            break;
    }
    if (i2c2_Open(BH1750_1_ADDRESS, ONE_TIME_LOW_RES_MODE) == OPENING_ERROR) {
        return OPENING_ERROR;
    } else
        return OPENING_SUCCESSFUL;
}

uint16_t BH1750_Data_Read(uint8_t Read_meas) {
    uint8_t raw_value[2];
    uint16_t lux_value;
    if (i2c2_gets(Read_meas, raw_value, 2) == GET_ERROR) {
        i2c_Error.Data_Read_error = GET_ERROR;
        return 0;
    } else
        i2c_Error.Data_Read_error = GET_SUCCESSFUL;
    lux_value = (raw_value[0] << 8) | raw_value[1];
    return (uint16_t) (lux_value / 1.2);
}

uint16_t BH1750_measure(uint8_t channel, uint8_t average) {
    uint8_t i = 0;
    uint16_t Data_lux = 0;
    uint32_t acum = 0;
    do {
        while (i2c_Error.i2c_count_error < MAX_ERROR_COUNT) {
            if (i2c_BH1750_open(channel) == OPENING_ERROR)
                i2c_Error.i2c_count_error++;
            else {
                i2c_Error.I2C_state = OPENING_SUCCESSFUL;
                i2c_Error.i2c_count_error = 0;
                break;
            }
        }
        if (i2c_Error.i2c_count_error >= MAX_ERROR_COUNT) {
            i2c_Error.I2C_state = OPENING_ERROR;
            i2c_Error.SENS_ERROR = channel;
            i2c_Error.i2c_count_error = 0;
            return 0;
        }
        __delay_ms(20);
        Data_lux = BH1750_Data_Read(Read_measurement_result);
        if (i2c_Error.Data_Read_error == GET_ERROR) {
            i2c_Error.I2C_state = GET_ERROR;
            i2c_Error.SENS_ERROR = channel;
            return 0;
        } else {
            acum += Data_lux;
            i++;
            i2c_Error.I2C_state = GET_SUCCESSFUL;
        }
    } while (i < average);
    i2c_Error.I2C_state = CORRECT;
    i2c_Error.SENS_ERROR = CORECT_SENS;
    return (uint16_t) (acum / average);
}