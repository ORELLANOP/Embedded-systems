
#ifndef MEMORY_H
#define MEMORY_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.11.26
     * Librer�a para manejo de la memoria EEPROM del PIC18f46k22.
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     */

    /* -------------------------------------------------------------------------
     *     Definicion de posiciones de memoria utilizadas en EEPROM
     * -------------------------------------------------------------------------
     */

#define STATUS_SYSTEM       0

    void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData);

    /* DATAEE_WriteByte escribe el byte que se le env�a, en la direcci�n de memoria
     * que se le pasa como par�metro. Recordar que la EEPROM para el PIC18f46k22 tiene
     * solo "1024 bytes" de espacio 
     */

    uint8_t DATAEE_ReadByte(uint16_t bAdd);

    /* La funci�n DATAEE_ReadByte lee el byte almacenado en la direcci�n que se le pasa como
     * par�metro
     */

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif // MEMORY_H