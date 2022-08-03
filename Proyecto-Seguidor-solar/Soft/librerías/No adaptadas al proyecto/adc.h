#ifndef ADC_H
#define	ADC_H

/* v15.3.19i
 * Libreria para el manejo del conversor analogico - digital interno del
 * microcontrolador.
 * Optimizado para las conexiones de la placa de la interface.
 */

#include <xc.h>

/*Seleccion del canal de conversion.*/
#define CHANNEL_0           0x00
#define CHANNEL_1           0x01
#define CHANNEL_3           0x03

/*Configura el conversor A/D para la placa de la interface.*/
void adc_init(void);

/*Devuelve el valor del conversor AD, del canal seleccionado.*/
unsigned int adc_getConv(unsigned int channel);

#endif	/* ADC_H */

