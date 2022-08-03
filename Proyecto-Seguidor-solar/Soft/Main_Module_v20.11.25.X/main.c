
#include "user.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/mcc.h"
//#include "demo_mode.h"
//#include "autonomous_mode.h"

void main(void) {
    uint8_t comm_received;
    SYSTEM_Initialize();
    /* Inicialización de EEPROM (solo la primera vez, luego se comenta) */
    //DATAEE_WriteByte(STATUS_SYSTEM, MANUAL_MODE);
    //syst_Stat.mode = DATAEE_ReadByte(STATUS_SYSTEM);
    INTERRUPT_GlobalInterruptHighEnable(); // Interrupción por alta prioridad habilitada
    INTERRUPT_GlobalInterruptLowEnable(); // Interrupción por baja prioridad habilitada
    init_App();
    syst_Stat.mode = AUTONOMOUS_MODE;    
    while (1) {
        request_light.Average_Sensor[0] = BH1750_measure(LIGHT_CHANNEL_1, 1);
        printf("%$L%d,%d,%d,%d;",request_light.Average_Sensor[0],20,800,1500);
        __delay_ms(1000);
        //comm_received = command_execution();
        switch (syst_Stat.mode) {
                break;
            case AUTONOMOUS_MODE:
                //autonomous_mode(comm_received);
                break;
            case DEMO_MODE:
                //demo_operation();
                break;
            case MANUAL_MODE:
                if (STATUS_Eng_A.status == ACTIVE_ENGINE)
                    azi_lim_control(STATUS_Eng_A.turn);
                if ((usart1_busy == 0) && (STATUS_Eng_A.status == INACTIVE_ENGINE) && (STATUS_Eng_V.status == INACTIVE_ENGINE))
                    SLEEP();
                break;
        }
    }
    return;
}