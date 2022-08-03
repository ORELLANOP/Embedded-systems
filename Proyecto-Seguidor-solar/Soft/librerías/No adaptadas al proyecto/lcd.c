#include <xc.h>
#include "lcd.h"

/* v15.3.18i
 * Libreria para manejar un modulo LCD de 2 lineas por 16 caracteres basada en
 * el controlador HD44780.
 * El Visualizador LCD esta conectado al PIC mediante los 6 bits de menor peso
 * del Puerto C, <RC3:RC0> para datos y <RC5:RC4> para las lineas de control.
 * Esta conexion esta optimizada para conectar el modulo a la placa de la
 * interface.
 */

void lcd_write(char c){
    char aux = c;
    aux = (aux & 0x70) >> 4;
    LATC = (LATC & 0xF8) | aux;
    if((c & 0x80) != 0)
        PORTCbits.RC5 = 1;
    else
        PORTCbits.RC5 = 0;
    LCD_STROBE;
    aux = c & 0x07;
    LATC = (LATC & 0xF8) | aux;
    if((c & 0x08) != 0)
        PORTCbits.RC5 = 1;
    else
        PORTCbits.RC5 = 0;
    LCD_STROBE;
    return;
}

void lcd_command(char c){
    LATAbits.LATA5 = 0;
    lcd_write(c);
    __delay_ms(2);
    return;
}

void lcd_init(void){
    ANSELAbits.ANSA5 = 0;
    ANSELCbits.ANSC2 = 0;
    ANSELCbits.ANSC5 = 0;
    TRISC = TRISC & 0xD8;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    LATC = LATC & 0xD8;
    __delay_ms(2);
    LATC = LATC | 0x03;
    LCD_STROBE;
    __delay_ms(5);
    LATC = LATC & 0xD8;
    __delay_us(100);
    LATC = LATC | 0x02;
    LCD_STROBE;
    __delay_us(100);
    lcd_command(0x28);
    lcd_command(DISPLAY_OFF);
    lcd_command(CLEAR);
    lcd_command(CURSOR_OUT);
    lcd_command(CURSOR_INC);
    return;
}

void lcd_putch(char c){
    LATAbits.LATA5 = 1;
    lcd_write(c);
    __delay_us(40);
    return;
}

void lcd_puts(const char *s){
    while(*s)
        lcd_putch(*s++);
    return;
}

void lcd_num(unsigned int num){
    char s[5];
    utoa(s, num, 10);
    lcd_puts(s);
    return;
}

void lcd_cgram(const char *s, char lcd_char){
    char count = 7;
    lcd_command(CGRAM | (lcd_char * 8));
    do{
        lcd_putch(*s++);
    }while(--count);
    lcd_command(CLEAR);
}

/*
 *
 */
