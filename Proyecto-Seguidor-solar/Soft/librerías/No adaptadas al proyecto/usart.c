#include <xc.h>
#include "usart.h"

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

static struct{
    char head;
    char tail;
    char buffer[TX_BUFFER_SIZE];
    char remaining;
}usartTx;

static struct{
    char head;
    char tail;
    char buffer[RX_BUFFER_SIZE];
    char count;
    char busy;
}usartRx;


void usart_init(void){
    ANSELCbits.ANSC6 = 0;   // Configuro RC6 como digital
    ANSELCbits.ANSC7 = 0;   // Configuro RC7 como digital
    BAUD1CON = 0x48;        // ABDOVF   = 0     -> No usamos el auto-baud
                            // RCIDL    = 1     -> Bandera de bit de start
                            // DTRXP    = 0     -> Pin de recepcion de datos no invertido
                            // CTXP     = 0     -> Estado inactivo de la transmision es alto
                            // BRG16    = 1     -> Genereador de velocidad de transmision de 16 bits
                            // WUE      = 0     -> Recepcion continua
                            // ABDEN    = 0     -> Detector de velocidad deshabilitado
    RC1STA = 0x90;          // SPEN     = 1     -> Habilito el puerto serie
                            // RX9      = 0     -> Recepcion de 8 bits
                            // SREN     = 0     -> No utilizado en recepcion asincrona
                            // CREN     = 1     -> Recepcion continua
                            // ADDEN    = 0     -> No se usa en modo de 8 bits
                            // FERR     = 0     -> Bit de error en la trama (solo lectura)
                            // OERR     = 0     -> Bit de error de sobreescritura (solo lectura)
                            // RX9D     = 0     -> Noveno bit de dato recibido (solo lectura)
    TX1STA = 0x26;          // CSRC     = 0     -> No se usa en recepcion asincrona
                            // TX9      = 0     -> Transmision de 8 bits
                            // TXEN     = 1     -> Habilito transmision
                            // SYNC     = 0     -> Modo asincrono
                            // SENDB    = 0     -> Sin envio de caracter de quiebre
                            // BRGH     = 1     -> Alta velocidad
                            // TRMT     = 1     -> Buffer de transmision libre (solo lectura)
                            // TX9D     = 0     -> Noveno bit de dato

    /*                     Fosc
     * Baud Rate = ------------------------- => <SPBRGH1:SPBRG1> = 1666
     *              4(<SPBRGH1:SPBRG1> + 1)
     */
    SPBRG1 = 0x82;
    SPBRGH1 = 0x06;
    TXREG1 = 0x00;          // Borro registro de transmision
    RCREG1 = 0x00;          // Borro registro de recepcion
    TRISCbits.TRISC7 = 1;
    usartTx.head = 0;
    usartTx.tail = 0;
    usartTx.remaining = sizeof(usartTx.buffer);
    usartRx.head = 0;
    usartRx.tail = 0;
    usartRx.count = 0;
    IPR1bits.RC1IP = 1;     // Interrupcion por recepcion de USART de alta prioridad
    IPR1bits.TX1IP = 0;     // Interrupcion por transmision de USART de baja prioridad
    PIR1bits.RC1IF = 0;     // Borro bandera de recepcion
    PIE1bits.RC1IE = 1;     // Habilito interrupcion por recepcion
    return;
}

char usart_read(void){
    char readValue = 0;
    while(usartRx.count == 0);
    PIE1bits.RC1IE = 0;
    readValue = usartRx.buffer[usartRx.tail++];
    if(sizeof(usartRx.buffer) <= usartRx.tail){
        usartRx.tail = 0;
    }
    usartRx.count--;
    PIE1bits.RC1IE = 1;
    return readValue;
}

char getche(void){
    return usart_read();
}

char usart_busy(void){
    char state;
    if(usartRx.busy > 0){
        state = 1;
        usartRx.busy--;
    }
    else
        state = 0;
    return state;
}

void usart_write(char dataTx){
    while(usartTx.remaining == 0);
    if(PIE1bits.TX1IE == 0){
        TXREG1 = dataTx;
    }
    else{
        PIE1bits.TX1IE = 0;
        usartTx.buffer[usartTx.head++] = dataTx;
        if(sizeof(usartTx.buffer) <= usartTx.head){
            usartTx.head = 0;
        }
        usartTx.remaining--;
    }
    PIE1bits.TX1IE = 1;
    return;
}

void putch(char dataTx){
    usart_write(dataTx);
    return;
}

void USART_TX_ISR(void){
    if(sizeof(usartTx.buffer) > usartTx.remaining){
        TXREG1 = usartTx.buffer[usartTx.tail++];
        if(sizeof(usartTx.buffer) <= usartTx.tail){
            usartTx.tail = 0;
        }
        usartTx.remaining++;
    }
    else{
        PIE1bits.TX1IE = 0;
    }
    return;
}

void USART_RX_ISR(void){
    if(RC1STAbits.OERR){
        RC1STAbits.SPEN = 0;
        RC1STAbits.SPEN = 1;
    }
    usartRx.buffer[usartRx.head] = RCREG1;
    TXREG = usartRx.buffer[usartRx.head];
    /* Corroborar si el ultimo caracter recibido es el caracter de fin de linea,
     * Se puede utilizar caulquier caracter para ello. */
    if(usartRx.buffer[usartRx.head] == 0x0D ||
       usartRx.buffer[usartRx.head] == '\r' ||     
       usartRx.buffer[usartRx.head] == '\n')        // ENTER
        usartRx.busy++;
    if(sizeof(usartRx.buffer) <= ++usartRx.head){
        usartRx.head = 0;
    }
    usartRx.count++;
    return;
}

/*
 *
 */

