
#ifndef DEVICE_CONFIG_H
#define	DEVICE_CONFIG_H

/* v20.11.26
 * Se configura al sistema mediante sus bits de configuraci�n, para trabajar con el oscilador
 * principal, basado a cristal de cuarzo de media potencia (4 a 16MHz), con PLL habilitado x4.
 * Switcheo de osciladores deshabilitado. Habilitado el Monitor de fallos en el reloj principal,
 * el temporizador de arranque que estima el tiempo de estabilizaci�n del cristal en 1024 ticks
 * m�s 2 ms extras, reseteo por baja de tensi�n en la alimentaci�n por debajo de un umbral de 2,2V,
 * por desborde en el stack. Pin f�sico de reset configurado, memorias sin protecci�n de lectura
 * externa (c�digo no protegido), perro guard�an deshabilitado. 
 */

#define _XTAL_FREQ 64000000

#endif	/* DEVICE_CONFIG_H */