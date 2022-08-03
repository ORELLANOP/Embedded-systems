#include <xc.h>
#include "key.h"

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

void key_init(void){
    TRISB = 0xF0;               // Configuro:
                                    // PORTB<7:4> entrada
                                    // PORTB<3:0> salida
    ANSELB = 0x00;              // Todo el puerto B como digital
    LATB = 0xF0;                // Los pines configurados como salida son puestos a cero
    INTCON2bits.nRBPU = 0;      // Habilito las resistencias de pull-up del puerto B
    WPUB = 0xF0;                // Habilito las resistencias de pull-up PORTB<7:4>
    INTCON2bits.RBIP = 0;       // Baja prioridad por interrupcion de cambio en PORTB<7:4>
    INTCONbits.RBIE = 1;        // Habilito interrupcion por cambio en PORTB<7:4>
    INTCONbits.RBIF = 0;        // Borro bandera de interrupcion por cambio en PORTB<7:4>
    return;
}

void RB_ISR(void){
    static const char shift[4] = {0xFE, 0xFD, 0xFB, 0xF7};
    static const char key_dec[16] = {'7', '8', '9', 'D', '4', '5', '6', 'C',
    '1', '2', '3', 'B', '*', '0', '#', 'A'};
    char i, key = '\0', scan;
    __delay_ms(5);
    for(i = 0; i < 4 && !key; i++){
        LATB = shift[i];
        scan = PORTB | 0x0F;
        switch(scan){
            case 0x7F:
                key = key_dec[i];
                break;
            case 0xBF:
                key = key_dec[i + 4];
                break;
            case 0xDF:
                key = key_dec[i + 8];
                break;
            case 0xEF:
                key = key_dec[i + 12];
                break;
        }
    }
    __delay_ms(5);                      // Demora para evitar rebotes
    LATB = 0xF0;
    while(PORTB != 0xF0);
    if(key != '\0'){                    // Si se presiono una tecla lo guardo
                                        // en la posicion mas baja del buffer
        keypad.flag = 1;
        for(i = 0; i < (BUFFER_KEY - 1); i++)
            keypad.buffer[(BUFFER_KEY - 1) - i] = keypad.buffer[(BUFFER_KEY - 2) - i];
        keypad.buffer[0] = key;
    }
    INTCONbits.RBIF = 0;
    return;
}

const char * key_read(void){
    if(keypad.flag){
        keypad.flag = 0;
        return keypad.buffer;
    }
    else{
        return '\0';
    }
}

/*
 *
 */
