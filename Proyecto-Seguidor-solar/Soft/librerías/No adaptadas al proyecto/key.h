#ifndef KEY_H
#define	KEY_H

/* v15.3.27i
 * Libreria para leer un teclado matricial de 4 filas por 4 columnas conectado
 * al Puerto B del microcontrolador. La funcion lee el teclado y devuelve el
 * valor de la tecla presionada. De no presionarse ninguna tecla al momento de
 * la lectura devuelve el caracter '\0'. El teclado debe estar conectado al
 * Puerto B con la siguiente disposicion:
 *
 *                      RB0 RB1 RB2 RB3
 *                       ^   ^   ^   ^
 *                     |---|---|---|---|
 *              RB4--> | C | 0 | = | + |
 *              RB5--> | 1 | 2 | 3 | - |
 *              RB6--> | 4 | 5 | 6 | * |
 *              RB7--> | 7 | 8 | 9 | / |
 *                     |---|---|---|---|
 *
 * Esta conexion esta optimizada para conectar el modulo a la placa de la
 * interface.
 */

#define _XTAL_FREQ  64000000
#define BUFFER_KEY  8
#include <xc.h>

static struct{
    char buffer[BUFFER_KEY];
    char flag;
}keypad;

/*Inicializa los pines y registros necesarios para la lectura del teclado. El
 * mismo sera leido por interupcion.*/
void key_init(void);

/**/
void RB_ISR(void);

/**/
const char * key_read(void);

#endif	/* KEY_H */

