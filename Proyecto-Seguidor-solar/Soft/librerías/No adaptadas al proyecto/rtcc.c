#include <xc.h>
#include "rtcc.h"

/* v15.5.9i
 * Libreria para el manejo de un reloj en tiempo real (RTCC). Se puede utilizar
 * tanto para el integrado DS1307 de Maxim como para el MCP79410 de Microchip.
 */

void rtcc_init(void){
    i2c_init();
    char data;
    char *ptrData = &data;
    
#ifdef DS1307
    data = 0x10;
    i2c_puts(DS1307, CONTROL, ptrData, 1);
#else
#ifdef MCP79410
    data = 0x40;
    i2c_puts(MCP79410, CONTROL, ptrData, 1);
#endif
#endif
    return;
}

void rtcc_gets(Timekeeper * timekeeper){
    char *ptrTime = (char *) timekeeper;
    
#ifdef DS1307
    i2c_gets(DS1307, SEC, ptrTime, YEAR);
#else
#ifdef MCP79410
    i2c_gets(MCP79410, SEC, ptrTime, YEAR);
#endif
#endif
    
    return;
}

void rtcc_puts(Timekeeper * timekeeper){
    char *ptrTime = (char *) timekeeper;
    
#ifdef DS1307
    i2c_puts(DS1307, SEC, ptrTime, YEAR);
#else
#ifdef MCP79410
    timekeeper->sec |= 0x80;
    i2c_puts(MCP79410, SEC, ptrTime, YEAR);
#endif
#endif
    
    return;
}
