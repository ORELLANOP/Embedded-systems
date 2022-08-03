
#ifndef I2C2_MASTER_H
#define I2C2_MASTER_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "tmr0.h"

/* v20.11.27
 * Librer�a creada para manejar el m�dulo MSSP2 en modo I2C2 Maestro.
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 * Cabe aclarar que la forma en que esta librer�a est� desarrollada, no contempla
 * el uso de interrupciones. 
 */

/* Macros para la comunicaci�n I2C2 */

#define i2c2Start            {SSP2CON2bits.SEN = 1; while(SSP2CON2bits.SEN);} // espera a ser borrada por hard (autom�ticamente)                         
#define i2c2Stop             {SSP2CON2bits.PEN = 1; while(SSP2CON2bits.PEN);} // espera a ser borrada por hard (autom�ticamente)  
#define i2c2Restart          {SSP2CON2bits.RSEN = 1; while(SSP2CON2bits.RSEN);} // espera a ser borrada por hard (autom�ticamente)  

/* Definiciones en general */

#define TMR0_i2c2_ctrl       25 // Para rutina de control IDLE

#define CORRECT              0
#define OPENING_ERROR        1  // Error al abrir la comunicaci�n, por no respuesta del esclavo
#define ACK_ERROR            2  // El eslavo no envi� el Acknowledge
#define GET_ERROR            3  // Error al obtener uno o mas datos
#define PUT_ERROR            4  // Error al intentar colocar un dato en el Slave
#define OPENING_SUCCESSFUL   5  // Apertura de la comunicaci�n exitosa
#define ACK_SUCCESSFUL       6  // Acknowledge enviado correctamente por el esclavo
#define GET_SUCCESSFUL       7  // Obtenci�n de datos exitosa
#define PUT_SUCCESSFUL       8  // Colocaci�n de datos en el Slave, exitosa

#define _IDLE                0  // Para bucle de control de rutina IDLE
#define _ACKOK               1  // Para bucle de control de rutina ACKNOWLEDGE
#define _RCEN                2  // Para bucle de control de rutina de RCEN

/* i2c2_init inicializa los registros del modulo MSSP1 en modo I2C2 maestro 
 * a 100kHz.
 */

void i2c2_init(void);

/* i2c2_Open abre la comunicaci�n con el dispositivo con el cual desea comunicarse
 * el uC. Esta funci�n es convocada por i2c2_gets o i2c2_puts.
 * Una vez convocada i2c2_gets o i2c2_puts, ambas ejecutan antes que nada el llamado
 * a i2c2_Open, pas�ndole como argumentos
 * slaveAddress: Direccion del dispositivo esclavo con el que se desea comunicar
 * address: Posicion de memoria del dispositivo esclavo desde donde se desea
 *          escribir los datos enviados (en algunos dispositivos este par�metro
 *          puede significar otra cosa, al margen de que el protocolo es igual para
 *          todos los dispositivos).
 * i2c2_Open mientras ejecuta la inicializaci�n de la comunicaci�n, va ejecutando una funci�n
 * que es i2c2_IDLE_control, con la cual verifica que exista una respuesta por parte del
 * dispositivo con el que el uC esta interactuando. En caso de exceder un tiempo que normalmente
 * es 25ms, la funci�n de control reporta con "false" que no hubo �xito en la comunicaci�n,
 * por lo que i2c2_Open generar� en su control una "reinicializaci�n" del MSSP1.
 * 
 * La funci�n i2c2_Open retorna un valor booleano, siendo "true" si la comunicaci�n se abri�
 * exitosamente, o "false" si ha fallado.
 */
uint8_t i2c2_Open(uint8_t slaveAddress, uint8_t address);

/* i2c2_gets lee los datos recibidos en el modulo MSSP. La funci�n debe recibir los
 * siguientes parametros:
 * slaveAddress: Direccion del dispositivo esclavo con el que se desea comunicar
 * *ptrData: Puntero a la variable, arreglo o estructura donde se desee guardar 
 *           los datos recibidos.
 * length: Cantidad de bytes que se desan leer.
 */
uint8_t i2c2_gets(uint8_t slaveAddress, uint8_t *ptrData, uint8_t length);

/* i2c2_puts se encarga de escribir datos a partir de la posici�n de memoria indicada
 * por la funci�n i2c2_ Open. Los par�metros que debe recibir son:
 * *ptrData: Puntero a la variable, arreglo o estructura donde se encuentran
 *           los datos que se desea escribir.
 * length: Cantidad de bytes que se desan leer.
 */

uint8_t i2c2_puts(uint8_t *ptrData, uint8_t length);

/* i2c2_control es la funci�n encargada de monitorear los tiempos de ocio
 * generados por la espera de un Acknowledge por parte de un dispositivo vinculado
 * a esta comunicaci�n I2C2. En caso de que ese tiempo se exceda, se deja de esperar y 
 * se reinicia la configuraci�n del BUS i2c2, de modo de arrancar nuevamente la comunicaci�n. 
 * Tambien puede monitorear al bit ACKOK y al RCEN, dependiendo si se le pasa por el argumento:
 * _IDLE, _ACKOK, _RCEN
 */

bool i2c2_control(uint8_t bit_field);

#endif //I2C2_MASTER_H