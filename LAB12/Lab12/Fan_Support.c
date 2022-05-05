#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;

#define OFF 0x00
#define RED 0x01
#define YELLOW 0x03
#define GREEN 0x02 
#define BLUE 0x04
#define PURPLE 0x05
#define CYAN 0x06
#define WHITE 0x07

char D2[7] = {RED, YELLOW, GREEN, BLUE, PURPLE, CYAN, WHITE};

int get_RPM()
{
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS 
}

void Toggle_Fan()
{
    if(FAN == 0) Turn_On_Fan();
    else Turn_Off_Fan();
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FAN_LED = 0;    
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_LED = 1;
}

void Increase_Speed()
{
    if(duty_cycle == 100)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(100);
    }
    else 
    {
        duty_cycle += 5;
        do_update_pwm(duty_cycle);
    }
}

void Decrease_Speed()
{
    if(duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(0);
    }
    else 
    {
        duty_cycle -= 5;
        do_update_pwm(duty_cycle);
    } 
}

void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ;                                      // set the frequency for 25 Khz
    T2CON = 0b00000111;
    dc_f = ( 4.0 * duty_cycle / 20.0) ;                     // calculate factor of duty cycle versus a 25 Khz signal
    dc_I = (int) dc_f;                                      // get the integer part
    if (dc_I > duty_cycle) dc_I++;                          // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2;
}

//void Set_DC_RGB(int duty_cycle)
//{
//    // add code
//}
//
//void Set_RPM_RGB(int rpm)
//{
//    //add code
//}

void Set_DC_RGB(int duty_cycle)
{
    // add code
//    if (duty_cycle >= 0 && duty_cycle < 9) SET_D1_OFF(); 
//    else if (duty_cycle >= 10 && duty_cycle < 19) SET_D1_RED(); 
//    else if (duty_cycle >= 20 && duty_cycle < 29) SET_D1_GREEN(); 
//    else if (duty_cycle >= 30 && duty_cycle < 39) SET_D1_YELLOW(); 
//    else if (duty_cycle >= 40 && duty_cycle < 49) SET_D1_BLUE();
//    else if (duty_cycle >= 50 && duty_cycle < 59) SET_D1_PURPLE(); 
//    else if (duty_cycle >= 60 && duty_cycle < 69) SET_D1_CYAN(); 
//    else if (duty_cycle >= 70) SET_D1_WHITE();
    
    if (duty_cycle >= 70)
        PORTD = ((PORTD & 0xF0) | 0x07) & 0xFF;
    else
        PORTD = ((PORTD & 0xF0) | duty_cycle/10) & 0xFF;
}

void Set_RPM_RGB(int rpm)
{
    //add code
//    if (rpm == 0) SET_D1_OFF(); 
//    else if (rpm > 0 && rpm < 500) SET_D2_RED(); 
//    else if (rpm >= 500 && rpm < 1000) SET_D2_YELLOW(); 
//    else if (rpm >= 1000 && rpm < 1500) SET_D2_GREEN(); 
//    else if (rpm >= 1500 && rpm < 2000) SET_D2_BLUE();
//    else if (rpm >= 2000 && rpm < 2500) SET_D2_PURPLE(); 
//    else if (rpm >= 2500 && rpm < 3000) SET_D2_CYAN(); 
//    else if (rpm >= 3500) SET_D2_WHITE();
    
    if (rpm < 1)
        PORTE = 0x00;
    else
        PORTE = D2[rpm / 500];
}


