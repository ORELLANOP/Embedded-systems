#ifndef USART_H
#define	USART_H

/* v15.3.26i
 * Libreria que permite implementar una comunicacion serie asincrona utilizando
 * el modulo interno USART del microcontrolador con las siguientes
 * caracteristicas:
 *
 *      - Velocidad de transmision: 9600 baudios
 *      - Un bit de inicio o Start
 *      - 8 bits de datos
 *      - Sin paridad
 *      - Un bits de final o Stop
 *      - Sin manejo de interrupciones
 */

#include <xc.h>
#include <stdio.h>

#define TX_BUFFER_SIZE  32      // Largo del buffer de transmision.
#define RX_BUFFER_SIZE  32      // Largo del buffer de recepcion.

/* Configura los registros necesario para utilizar la USART. */
void usart_init(void);

/* Lee el dato del buffer de entrada y lo devuelve. */
char usart_read(void);

/* Comprueba se ha llegado el caracter de fin de la cadena de caracteres */
char usart_busy(void);

/* Escribe el caracter enviado en el buffer de transmision. */
void usart_write(char dataTx);

/* Funcion que debe ser incluida en el vector de interrupcion que se encarga de
 * transeferir del buffer de transmision al registro TXREG1 los datos. */
void USART_TX_ISR(void);

/* Funcion que debe ser incluida en el vector de interrupcion que se encarga de
 * transeferir los datos del registro RCREGx al buffer de recepcion. */
void USART_RX_ISR(void);

#endif	/* USART_H */

