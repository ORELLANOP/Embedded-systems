
#ifndef ADC_H
#define ADC_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "device_config.h"

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.11.26
     * Librería para el manejo del módulo ADC del PIC18F46K22.
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     * Se destina su uso a la conversión de tensiones provenientes de diferentes 
     * sensores: posición azimutal tipo potenciómetro, tensión de panel, corriente en la 
     * carga por medio de sensor de efecto Hall "Allegro".
     */

    /* Canales utilizados para este proyecto */

#define I_PANEL             0x00  // Sensado de corriente del panel
#define ANG_AZIMUT          0x01  // Sensor de posición de ángulo azimut  
#define V_PANEL             0x02  // Sensor de corriente

    /* ADC_Initialize es la función que configura el dispositivo, así como todos los posibles
     * canales analógicos a utilizar como fuente de conversión.
     */

    void ADC_Initialize(void);

    /* ADC_GetConversión devuelve el valor de una o más conversiones dependiendo
     * de si se pide o no promediar una cantidad de ellas. El parámetro "channel" 
     * selecciona el canal, mientras que el parámetro "averaged" indica la cantidad de muestras
     * que se tomará de ese canal y que luego se promediarán.
     */

    uint16_t ADC_GetConversion(uint8_t channel, uint8_t averaged);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif	//ADC_H