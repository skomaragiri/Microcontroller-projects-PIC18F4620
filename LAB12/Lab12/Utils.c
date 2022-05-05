#include <p18f4620.h>
#include "Main.h"
#include "Utils.h"
#include "stdio.h"

#include "Fan_Support.h"

char duty_cycle;
#define key_pressed PORTBbits.RB7
void Do_Beep()
{
    key_pressed = 1;
    Activate_Buzzer();
    Wait_One_Sec();
    Deactivate_Buzzer();
    key_pressed = 0;
    do_update_pwm(duty_cycle);
}

void Wait_One_Sec()
{
    for(int I = 0; I < 17000; I++);       //for loop counter used to create the delay 
}

void Activate_Buzzer()
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0;    
}

