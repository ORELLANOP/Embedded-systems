
#include "eusart1.h"

/* v20.11.26
 * Librería para el manejo del módulo EUSART1 del PIC18F46K22, destinando
 * su diseño a la comunicación con un módulo WiFi. 
 * Optimizada para las conexiones de la placa "Main_Module v20.11.23".
 */

/* Variables para manejo de transmisión por Eusart 1*/

volatile uint8_t eusart1TxHead = 0; //cabeza
volatile uint8_t eusart1TxTail = 0; // cola
volatile uint8_t eusart1TxBuffer[EUSART1_TX_BUFFER_SIZE]; // array
volatile uint8_t eusart1TxBufferRemaining; // restante

/* Variables para manejo de recepción por Eusart 1*/

volatile uint8_t eusart1RxHead = 0;
volatile uint8_t eusart1RxTail = 0;
volatile uint8_t eusart1RxBuffer[EUSART1_RX_BUFFER_SIZE];

/* Variables para control del estado de la recepción */

volatile eusart1_status_t eusart1RxStatusBuffer[EUSART1_RX_BUFFER_SIZE];
volatile uint8_t eusart1RxCount;
volatile eusart1_status_t eusart1RxLastError;

bool DataRx1 = false; /* Para habilitar o deshabilitar RX dependiendo del 
                                * caracter de inicio de comunicación 
                                */
uint8_t caracter_count = 0;

/* Variable agregada para dar funcionalidad a la recepción de strings */

volatile uint8_t usart1_busy;

/* Section: EUSART1 APIs */

void (*EUSART1_TxDefaultInterruptHandler)(void);
void (*EUSART1_RxDefaultInterruptHandler)(void);

void (*EUSART1_FramingErrorHandler)(void);
void (*EUSART1_OverrunErrorHandler)(void);
void (*EUSART1_ErrorHandler)(void);

void EUSART1_DefaultFramingErrorHandler(void);
void EUSART1_DefaultOverrunErrorHandler(void);
void EUSART1_DefaultErrorHandler(void);

void EUSART1_Initialize(void) {
    ANSELC &= 0x3F; // Configuro como digital RC6 y RC7 
    PIE1bits.RC1IE = 0; // disable interrupts before changing states
    EUSART1_SetRxInterruptHandler(EUSART1_Receive_ISR);
    PIE1bits.TX1IE = 0;
    EUSART1_SetTxInterruptHandler(EUSART1_Transmit_ISR);
    BAUDCON1 = 0x08; /* ABDOVF no_overflow; 
                      * CKTXP async_noninverted_sync_fallingedge; 
                      * BRG16 16bit_generator; 
                      * WUE disabled; 
                      * ABDEN disabled; 
                      * DTRXP not_inverted;
                      */
    RCSTA1 = 0x90; /* SPEN enabled; 
                    * RX9 8-bit; 
                    * CREN enabled; 
                    * ADDEN disabled; 
                    * SREN disabled; 
                    */
    TXSTA1 = 0x24; /* TX9 8-bit; 
                    * TX9D 0; 
                    * SENDB sync_break_complete; 
                    * TXEN enabled; 
                    * SYNC asynchronous; 
                    * BRGH hi_speed; 
                    * CSRC slave_mode;
                    */
    SPBRG1 = 0x82;
    SPBRGH1 = 0x06;
    TRISCbits.TRISC7 = 1; //Configuro como entrada el pin RC7 de recepción    
    EUSART1_SetFramingErrorHandler(EUSART1_DefaultFramingErrorHandler);
    EUSART1_SetOverrunErrorHandler(EUSART1_DefaultOverrunErrorHandler);
    EUSART1_SetErrorHandler(EUSART1_DefaultErrorHandler);
    eusart1RxLastError.status = 0;

    eusart1TxHead = 0;
    eusart1TxTail = 0;
    eusart1TxBufferRemaining = sizeof (eusart1TxBuffer);

    eusart1RxHead = 0;
    eusart1RxTail = 0;
    eusart1RxCount = 0;
          
    PIE1bits.RC1IE = 1; // Habilito la interrupción por recepción de EUSART
    return;
}

bool EUSART1_is_tx_ready(void) {
    return (eusart1TxBufferRemaining ? true : false);
}

bool EUSART1_is_rx_ready(void) {
    bool state;
    if (usart1_busy > 0) {
        state = 1;
        usart1_busy--;
    } else
        state = 0;
    return (state ? true : false);
}

/*bool EUSART1_is_tx_done(void) {
    return TXSTA1bits.TRMT;
}*/

eusart1_status_t EUSART1_get_last_status(void) {
    return eusart1RxLastError;
}

uint8_t EUSART1_Read(void) {
    uint8_t readValue = 0;
    while (0 == eusart1RxCount);
    eusart1RxLastError = eusart1RxStatusBuffer[eusart1RxTail];
    readValue = eusart1RxBuffer[eusart1RxTail++];
    if (sizeof (eusart1RxBuffer) <= eusart1RxTail) {
        eusart1RxTail = 0;
    }
    PIE1bits.RC1IE = 0;
    eusart1RxCount--;
    PIE1bits.RC1IE = 1;
    return readValue;
}

char getche(void) {
    return EUSART1_Read();
}

void EUSART1_Write(uint8_t txData) {
    while (0 == EUSART1_is_tx_ready()) { // En caso de no haber datos...espero
    }
    /* Por defecto la interrupción de Tx se deshabilita. Por ello, 
     * si se cumple la siguiente condición...
     */
    if (0 == PIE1bits.TX1IE) {
        /* se envía directamente al registro de transmisión "TSR" el primer byte
         * según establece el protocolo. Sin embargo aquí no se borra aún el flag 
         * de Tx, pues aún no se llenó nuevamente TXREG de modo que quede llena
         * la cola de salida. 
         * El flag de TX es de solo lectura, y se setea automáticamente si el 
         * módulo TX está habilitado, reseteandose si TXREG se escribe o si el 
         * EUSART se encuentra en una transacción.
         */
        TXREG1 = txData;
    } else {/* sino es TX1IE != 0 (interrupción por Tx ya habilitada)...
             */
        PIE1bits.TX1IE = 0; // deshabilito int por Tx
        eusart1TxBuffer[eusart1TxHead++] = txData; /* Se va incrementando la 
                                                    * posición de "Head" para
                                                    * almacenar los caracteres
                                                    * restantes.
                                                    */
        if (sizeof (eusart1TxBuffer) <= eusart1TxHead)
            eusart1TxHead = 0;
        eusart1TxBufferRemaining--;
        /* Luego de cargados los dos primeros bytes en la usart, no se envían 
         * más datos hasta tener lleno el buffer de transmisión.
         */
    }
    PIE1bits.TX1IE = 1;
    return;
}

void putch(char txData) {
    return EUSART1_Write(txData);
}

void EUSART1_Transmit_ISR(void) {
    if (sizeof (eusart1TxBuffer) > eusart1TxBufferRemaining) { /* El tamaño del Buffer 
                                                                * es mayor al remanente
                                                                * por transmitir en 
                                                                * el buffer?
                                                                */
        TXREG1 = eusart1TxBuffer[eusart1TxTail++];
        if (sizeof (eusart1TxBuffer) <= eusart1TxTail)
            eusart1TxTail = 0;
        eusart1TxBufferRemaining++;
    } else
        PIE1bits.TX1IE = 0;
    return;
}

void EUSART1_Receive_ISR(void) {
    eusart1RxStatusBuffer[eusart1RxHead].status = 0;
    if (RCSTA1bits.FERR) {
        eusart1RxStatusBuffer[eusart1RxHead].ferr = 1;
        EUSART1_FramingErrorHandler();
    }
    if (RCSTA1bits.OERR) {
        eusart1RxStatusBuffer[eusart1RxHead].oerr = 1;
        EUSART1_OverrunErrorHandler();
    }
    if (eusart1RxStatusBuffer[eusart1RxHead].status) {
        EUSART1_ErrorHandler();
    } else {
        EUSART1_RxDataHandler();
    }
    return;
}

void EUSART1_RxDataHandler(void) {
    if (RCREG1 == 58 && DataRx1 == 0) { // : = 58, condición de inicio
        DataRx1 = 1;
        caracter_count = 0;
    } else if (DataRx1) {
        eusart1RxBuffer[eusart1RxHead] = RCREG1;
        if (eusart1RxBuffer[eusart1RxHead] == 59) { /* "; = 59" es el caracter 
                                                     * de fin de mensaje.
                                                     */
            usart1_busy++;
            eusart1RxBuffer[eusart1RxHead] = '\0';
            DataRx1 = 0;
        }
        if (sizeof (eusart1RxBuffer) <= ++eusart1RxHead)
            eusart1RxHead = 0;
        if (caracter_count >= sizeof (eusart1RxBuffer))
            DataRx1 = 0;
        else
            caracter_count++;
        eusart1RxCount++;
    } else
        DataRx1 = 0;
    return;
}

void EUSART1_DefaultFramingErrorHandler(void) {
    // Agregar código en caso de tener Error de trama en recepción
}

void EUSART1_DefaultOverrunErrorHandler(void) {
    // EUSART1 error - restart
    RCSTA1bits.CREN = 0;
    RCSTA1bits.CREN = 1;
}

void EUSART1_DefaultErrorHandler(void) {
    EUSART1_RxDataHandler();
}

void EUSART1_SetFramingErrorHandler(void (* interruptHandler)(void)) {
    EUSART1_FramingErrorHandler = interruptHandler;
}

void EUSART1_SetOverrunErrorHandler(void (* interruptHandler)(void)) {
    EUSART1_OverrunErrorHandler = interruptHandler;
}

void EUSART1_SetErrorHandler(void (* interruptHandler)(void)) {
    EUSART1_ErrorHandler = interruptHandler;
}

void EUSART1_SetTxInterruptHandler(void (* interruptHandler)(void)) {
    EUSART1_TxDefaultInterruptHandler = interruptHandler;
}

void EUSART1_SetRxInterruptHandler(void (* interruptHandler)(void)) {
    EUSART1_RxDefaultInterruptHandler = interruptHandler;
}