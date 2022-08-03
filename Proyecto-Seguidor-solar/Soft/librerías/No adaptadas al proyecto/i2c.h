#ifndef I2C_H
#define	I2C_H

/* v15.5.17i
 * Libreria para el manejo del modulo de comunicacion serie MSSP en modo I2C
 * maestro.
 */

#include <xc.h>

/* Marcos utilizado en la libreria. */
#define i2cIdle             while((SSP1CON2 & 0x1F) || (SSP1STATbits.R_nW));
#define i2cStart            {SSP1CON2bits.SEN = 1; while(SSP1CON2bits.SEN);}
#define i2cStop             SSP1CON2bits.PEN = 1;
#define i2cRestart          {SSP1CON2bits.RSEN = 1; while(SSP1CON2bits.RSEN);}
#define i2cAckOk            while(SSP1CON2bits.ACKSTAT);

/* Inicializa los registros del modulo MSSP1 en modo I2C maestro a 100kHz.*/
void i2c_init(void);

/* Lee los datos recibidos en el modulo MSSP. La funcion debe recibir los
 * siguientes parametros:
 * slaveAddress: Direccion del dispositivo esclavo con el que se desea comunicar
 * address: Posicion de memoria del dispositivo esclavo desde donde se desea leer
 * *ptrData: Puntero a la estructura donde se guardaran los datos recibidos
 * length: Cantidad de bytes que se desan leer
 */
void i2c_gets(char slaveAddress, char address, char *ptrData, char length);

/* Envia los datos al modulo MSSP. La funcion debe recibir los siguientes
 * parametros:
 * slaveAddress: Direccion del dispositivo esclavo con el que se desea comunicar
 * address: Posicion de memoria del dispositivo esclavo desde donde se desea
 *          escribir los datos enviados
 * *ptrData: Puntero a la estructura donde estan guardados los datos a enviar
 * length: Cantidad de bytes que se desan enviar
 */
void i2c_puts(char slaveAddress, char address, char *ptrData, char length);

#endif	/* I2C_H */

