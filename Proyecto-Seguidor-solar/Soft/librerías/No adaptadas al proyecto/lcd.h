#ifndef LCD_H
#define LCD_H

/* v15.3.18i
 * Libreria para manejar un modulo LCD de 2 lineas por 16 caracteres basada en
 * el controlador HD44780.
 * El Visualizador LCD esta conectado al PIC mediante los 6 bits de menor peso
 * del Puerto C, <RC3:RC0> para datos y <RC5:RC4> para las lineas de control.
 * Esta conexion esta optimizada para conectar el modulo a la placa de la
 * interface.
 */

#define _XTAL_FREQ  64000000
#include <xc.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

#define LCD_STROBE      {LATAbits.LATA4 = 1; __delay_us(10); LATAbits.LATA4 = 0;}

////////////////////////////////////////////////////////////////////////////////

/* Lista de comandos para manejar las caracteristicas del LCD. */

#define CLEAR           0x01  // Borra pantalla y cursor en primera posicion
#define CGRAM           0x40  // Indica al LCD que se va a grabar en la CGRAM

// Caracteristicas de escritura de datos:
#define CURSOR_DEC      0x04  // Decremento posicion cursor, texto fijo
#define CURSOR_INC      0x06  // Incremento posicion cursor, texto fijo
#define TEXT_DEC        0x05  // Mueve el texto a la izquierda, al escribir un nuevo caracter
#define TEXT_INC        0x07  // Mueve el texto a la derecha, al escribir un nuevo caracter

// Control de la pantalla:
#define DISPLAY_OFF     0x08  // Apaga display
#define CURSOR_OUT      0x0C  // Enciende display, no se visuliza el cursor
#define CURSOR_IN       0x0E  // Enciende display, se visualiza el cursor
#define CURSOR_BLINK    0x0F  // Enciende display, efecto de parapadeo para el cursor

// Control de desplazamiento del cursor y display:
#define CURSOR_LEFT     0x10  // Desplaza el cursor a la izquierda
#define CURSOR_RIGHT    0x14  // Desplaza el cursor a la derecha
#define DISPLAY_LEFT    0x18  // Desplaza el display a la izquierda
#define DISPLAY_RIGHT   0x1C  // Desplaya el display a la derecha

// Posicion del cursor en pantalla:
#define LINE_1          0x80  // Coloca el cursor en la 1º linea del LCD
#define LINE_2          0xC0  // Coloca el cursor en la 2º linea del LCD

// Posicion de los caracteres especiales definidos por el usuario de la CGRAM
#define CHARACTER_1     0x00  // 1º Caracter en la CGRAM
#define CHARACTER_2     0x01  // 2º Caracter en la CGRAM
#define CHARACTER_3     0x02  // 3º Caracter en la CGRAM
#define CHARACTER_4     0x03  // 4º Caracter en la CGRAM
#define CHARACTER_5     0x04  // 5º Caracter en la CGRAM
#define CHARACTER_6     0x05  // 6º Caracter en la CGRAM
#define CHARACTER_7     0x06  // 7º Caracter en la CGRAM
#define CHARACTER_8     0x07  // 8º Caracter en la CGRAM

/* Rutina que nos permite enviar un dato o una orden al modulo LCD. */
void lcd_write(char c);

/* Rutina para enviar comandos al modulo LCD. */
void lcd_command(char c);

/* Rutina para configurar los pines del microcontrolador y el modo de
 * funcionamiento del LCD. */
void lcd_init(void);

/* Escribe un caracter en el LCD. */
void lcd_putch(char c);

/* Escribe un cadena de caracteres en el LCD. */
void lcd_puts(const char *s);

/* Rutina para enviar un numero al LCD. */
void lcd_num(unsigned int num);

/* Genera un nuevo caracter definido por el usuario en la CGRAM. Se puede crear
 * un maximo de 8 caracteres personalizados, los cuales deberan ser escritos
 * cada vez de inicie el modulo LCD. */
void lcd_cgram(const char *s, char lcd_char);

#endif
