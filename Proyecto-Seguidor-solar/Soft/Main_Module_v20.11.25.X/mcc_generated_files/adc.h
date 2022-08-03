
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
     * Librer�a para el manejo del m�dulo ADC del PIC18F46K22.
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     * Se destina su uso a la conversi�n de tensiones provenientes de diferentes 
     * sensores: posici�n azimutal tipo potenci�metro, tensi�n de panel, corriente en la 
     * carga por medio de sensor de efecto Hall "Allegro".
     */

    /* Canales utilizados para este proyecto */

#define I_PANEL             0x00  // Sensado de corriente del panel
#define ANG_AZIMUT          0x01  // Sensor de posici�n de �ngulo azimut  
#define V_PANEL             0x02  // Sensor de corriente

    /* ADC_Initialize es la funci�n que configura el dispositivo, as� como todos los posibles
     * canales anal�gicos a utilizar como fuente de conversi�n.
     */

    void ADC_Initialize(void);

    /* ADC_GetConversi�n devuelve el valor de una o m�s conversiones dependiendo
     * de si se pide o no promediar una cantidad de ellas. El par�metro "channel" 
     * selecciona el canal, mientras que el par�metro "averaged" indica la cantidad de muestras
     * que se tomar� de ese canal y que luego se promediar�n.
     */

    uint16_t ADC_GetConversion(uint8_t channel, uint8_t averaged);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif	//ADC_H