
#ifndef EUSART1_H
#define EUSART1_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

    /* v20.11.26
     * Librería para el manejo del módulo EUSART1 del PIC18F46K22, destinando
     * su diseño a la comunicación con un módulo WiFi. 
     * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
     */

#define EUSART1_TX_BUFFER_SIZE 24
#define EUSART1_RX_BUFFER_SIZE 24      

    /* Estructura de status en recepción */

    typedef union {

        struct {
            unsigned perr : 1;
            unsigned ferr : 1;
            unsigned oerr : 1;
            unsigned reserved : 5;
        };
        uint8_t status;
    } eusart1_status_t;

    extern volatile uint8_t eusart1TxBufferRemaining;
    extern volatile uint8_t eusart1RxCount;

    /* Variable agregada para dar funcionalidad a la recepción de strings */

    volatile uint8_t usart1_busy;

    /* Section: EUSART2 APIs */

    extern void (*EUSART1_TxDefaultInterruptHandler)(void);
    extern void (*EUSART1_RxDefaultInterruptHandler)(void);

    void EUSART1_Initialize(void);

    bool EUSART1_is_tx_ready(void);

    bool EUSART1_is_rx_ready(void);

    /* Indica si el Transmit Shift Register (TSR) está vacío (empty)
     * o lleno (full). Es decir indica si es posible o no realizar una transmisión
     */

    //bool EUSART1_is_tx_done(void);

    eusart1_status_t EUSART1_get_last_status(void);

    uint8_t EUSART1_Read(void);

    void EUSART1_Write(uint8_t txData);

    void EUSART1_Transmit_ISR(void);

    void EUSART1_Receive_ISR(void);

    void EUSART1_RxDataHandler(void);

    void EUSART1_SetFramingErrorHandler(void (* interruptHandler)(void));

    void EUSART1_SetOverrunErrorHandler(void (* interruptHandler)(void));

    void EUSART1_SetErrorHandler(void (* interruptHandler)(void));

    void EUSART1_SetTxInterruptHandler(void (* interruptHandler)(void));

    void EUSART1_SetRxInterruptHandler(void (* interruptHandler)(void));


#ifdef __cplusplus  // Provide C++ Compatibility

#endif

#endif  // EUSART1_H