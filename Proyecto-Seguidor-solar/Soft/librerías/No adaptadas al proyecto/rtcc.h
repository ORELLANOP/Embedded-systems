#ifndef RTCC_H
#define	RTCC_H

/* v15.5.9i
 * Libreria para el manejo de un reloj en tiempo real (RTCC). Se puede utilizar
 * tanto para el integrado DS1307 de Maxim como para el MCP79410 de Microchip.
 */

#include <xc.h>
#include "i2c.h"

/* Seleccion del chip a utilizar, marcar como comentario el que no se utilice */
#define DS1307              0xD0
//#define MCP79410            0xDE

/* Posicion de memoria donde se ubican los datos: */
#define SEC                 0x00    // Segundos (y control oscilador en MCP79410)
#define MIN                 0x01    // Minutos
#define HOUR                0x02    // Horas y control 12/24
#define DAY                 0x03    // Dia de la semana (1 = domingo al 7 = sabado)
                                    // En MCP79410, estado oscilador y alimentacion,
                                    // y habilitacion de entrada de bateria
#define DATE                0x04    // Dia del mes
#define MONTH               0x05    // Mes (en MCP79410 indicador de año bisiesto)
#define YEAR                0x06    // Año
#define CONTROL             0x07    // Control de alarma y señal de sincronizacion

/* Tipo de datos utilizado para el manejo de los datos. */
typedef struct{
    char sec;
    char min;
    char hour;
    char day;
    char date;
    char month;
    char year;
}Timekeeper;

/* Inicializo los parametros del RTCC y del modulo I2C del microcontrolador. */
void rtcc_init(void);

/* Envia los datos indicados por medio de un puntero a estructura (Timekeeper)
 * al RTCC. */
void rtcc_puts(Timekeeper * ptrTimekeeper);

/* Lee todos los registros del RTCC y son almacenados en la estructura indicada
 * por ptrTimekeeper. */
void rtcc_gets(Timekeeper * ptrTimekeeper);

#endif	/* RTCC_H */
