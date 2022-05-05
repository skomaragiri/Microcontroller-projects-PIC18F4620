#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C_Soft.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE


#define D1_RED PORTDbits.RD0                                                    //Definitions to simplify coding by assigning a name to a specific bit of the portB
#define D1_GREEN PORTDbits.RD1                                                  //Definitions to simplify coding by assigning a name to a specific bit of the portB
#define D1_BLUE PORTDbits.RD2                                                   //Definitions to simplify coding by assigning a name to a specific bit of the portB

#define D3_RED PORTDbits.RD4                                                    //Definitions to simplify coding by assigning a name to a specific bit of the portB
#define D3_GREEN PORTDbits.RD5                                                  //Definitions to simplify coding by assigning a name to a specific bit of the portB
#define D3_BLUE PORTDbits.RD6                                                   //Definitions to simplify coding by assigning a name to a specific bit of the portB

#define D2_RED PORTEbits.RE0                                                    //Definitions to simplify coding by assigning a name to a specific bit of the portA
#define D2_GREEN PORTEbits.RE1                                                  //Definitions to simplify coding by assigning a name to a specific bit of the portA
#define D2_BLUE PORTEbits.RE2                                                   //Definitions to simplify coding by assigning a name to a specific bit of the portB

void Initialize_Screen();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char setup_fan_temp = 75;
unsigned char Nec_state = 0;

short nec_ok = 0;
char Nec_code1;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;

char buffer[33]     = " ECE3301L Sp'21 L12\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_SW_Txt[]   = "OFF";                // text storage for Heater Mode

#define D3R             0x10                                                    // Hex value to turn the D3 LED RED
#define D3G             0x20                                                    // Hex value to turn the D3 LED GREEN
#define D3B             0x40                                                    // Hex value to turn the D3 LED BLUE
#define D3M             0x50                                                    // Hex value to turn the D3 LED MAGENTA
#define D3W             0x70                                                    // Hex value to turn the D3 LED WHITE

char D3[21] = {                                                                 // Array corresponding to the location of the buttons on the remote 
               D3R, D3R, D3R,                                                       // Used for the LED D3 which corresponds to the buttons in the leftmost column
               D3B, D3B, D3G,                                                       // Using defined hex values displays color of remote button on correct LED
               D3M, D3M, D3M,
               D3W, D3W, D3W,
               D3W, D3W, D3W,
               D3W, D3W, D3W,
               D3W, D3W, D3W
};

//void SET_D1_OFF(void)                                                           // Function for setting output for RGB LED D1 to OFF using previously established bit definitions 
//{
//    D1_RED = 0;
//    D1_GREEN = 0;
//    D1_BLUE = 0;
//}
//
//void SET_D1_RED(void)                                                           // Function for setting output for RGB LED D1 to RED using previously established bit definitions                  
//{
//    D1_RED = 1;
//    D1_GREEN = 0;
//    D1_BLUE = 0;
//}
//void SET_D1_GREEN(void)                                                         // Function for setting output for RGB LED D1 to GREEN using previously established bit definitions 
//{
//    D1_RED = 0;
//    D1_GREEN = 1;
//    D1_BLUE = 0;
//}
//void SET_D1_YELLOW(void)                                                        // Function for setting output for RGB LED D1 to YELLOW using previously established bit definitions 
//{
//    D1_RED = 1;
//    D1_GREEN = 1;
//    D1_BLUE = 0;
//}
//void SET_D1_BLUE(void)                                                          // Function for setting output for RGB LED D1 to BLUE using previously established bit definitions 
//{
//    D1_RED = 0;
//    D1_GREEN = 0;
//    D1_BLUE = 1;
//}
//void SET_D1_PURPLE(void)                                                        // Function for setting output for RGB LED D1 to PURPLE using previously established bit definitions 
//{
//    D1_RED = 1;
//    D1_GREEN = 0;
//    D1_BLUE = 1;
//}
//void SET_D1_CYAN(void)                                                          // Function for setting output for RGB LED D1 to CYAN using previously established bit definitions 
//{
//    D1_RED = 0;
//    D1_GREEN = 1;
//    D1_BLUE = 1;
//}
//void SET_D1_WHITE(void)                                                         // Function for setting output for RGB LED D1 to WHITE using previously established bit definitions 
//{
//    D1_RED = 1;
//    D1_GREEN = 1;
//    D1_BLUE = 1;
//}
//
//
//void SET_D2_OFF(void)                                                           // Function for setting output for RGB LED D2 to OFF using previously established bit definitions 
//{
//    D2_RED = 0;
//    D2_GREEN = 0;
//    D2_BLUE = 0;
//}
//
//void SET_D2_RED(void)                                                           // Function for setting output for RGB LED D2 to RED using previously established bit definitions 
//{
//    D2_RED = 1;
//    D2_GREEN = 0;
//    D2_BLUE = 0;
//}
//void SET_D2_GREEN(void)                                                         // Function for setting output for RGB LED D2 to GREEN using previously established bit definitions 
//{
//    D2_RED = 0;
//    D2_GREEN = 1;
//    D2_BLUE = 0;
//}
//void SET_D2_YELLOW(void)                                                        // Function for setting output for RGB LED D2 to YELLOW using previously established bit definitions 
//{
//    D2_RED = 1;
//    D2_GREEN = 1;
//    D2_BLUE = 0;
//}
//void SET_D2_BLUE(void)                                                          // Function for setting output for RGB LED D2 to BLUE using previously established bit definitions 
//{
//    D2_RED = 0;
//    D2_GREEN = 0;
//    D2_BLUE = 1;
//}
//void SET_D2_PURPLE(void)                                                        // Function for setting output for RGB LED D2 to PURPLE using previously established bit definitions 
//{
//    D2_RED = 1;
//    D2_GREEN = 0;
//    D2_BLUE = 1;
//}
//void SET_D2_CYAN(void)                                                          // Function for setting output for RGB LED D2 to CYAN using previously established bit definitions 
//{
//    D2_RED = 0;
//    D2_GREEN = 1;
//    D2_BLUE = 1;
//}
//void SET_D2_WHITE(void)                                                         // Function for setting output for RGB LED D2 to WHITE using previously established bit definitions 
//{
//    D2_RED = 1;
//    D2_GREEN = 1;
//    D2_BLUE = 1;
//}
//
//void SET_D3_OFF(void)                                                           // Function for setting output for RGB LED D3 to OFF using previously established bit definitions 
//{
//    D3_RED = 0;
//    D3_GREEN = 0;
//    D3_BLUE = 0;
//}
//
//void SET_D3_RED(void)                                                           // Function for setting output for RGB LED D3 to RED using previously established bit definitions 
//{
//    D3_RED = 1;
//    D3_GREEN = 0;
//    D3_BLUE = 0;
//}
//void SET_D3_GREEN(void)                                                         // Function for setting output for RGB LED D3 to GREEN using previously established bit definitions 
//{
//    D3_RED = 0;
//    D3_GREEN = 1;
//    D3_BLUE = 0;
//}
//void SET_D3_YELLOW(void)                                                        // Function for setting output for RGB LED D3 to YELLOW using previously established bit definitions 
//{
//    D3_RED = 1;
//    D3_GREEN = 1;
//    D3_BLUE = 0;
//}
//void SET_D3_BLUE(void)                                                          // Function for setting output for RGB LED D2 to BLUE using previously established bit definitions 
//{
//    D3_RED = 0;
//    D3_GREEN = 0;
//    D3_BLUE = 1;
//}
//void SET_D3_PURPLE(void)                                                        // Function for setting output for RGB LED D2 to PURPLE using previously established bit definitions 
//{
//    D3_RED = 1;
//    D3_GREEN = 0;
//    D3_BLUE = 1;
//}
//void SET_D3_CYAN(void)                                                          // Function for setting output for RGB LED D2 to CYAN using previously established bit definitions 
//{
//    D3_RED = 0;
//    D3_GREEN = 1;
//    D3_BLUE = 1;
//}
//void SET_D3_WHITE(void)                                                         // Function for setting output for RGB LED D2 to WHITE using previously established bit definitions 
//{
//    D3_RED = 1;
//    D3_GREEN = 1;
//    D3_BLUE = 1;
//}



char array1[21]={                                                                   // Array containing the hex data byte gathered from Tera Term indexed corresponding
                 0xa2, 0x62, 0xe2,                                                  // to the buttons on the remote starting at button 0 in the top left (CH-)
                 0x22, 0x02, 0xc2,                                                  // and reading from left to right to the bottom right button (9) 
                 0xe0, 0xa8, 0x90,
                 0x68, 0x98, 0xb0,
                 0x30, 0x18, 0x7a,
                 0x10, 0x38, 0x5a,
                 0x42, 0x4a, 0x52
                };
char txt1[21][4] ={                                                                 // Array containing the text inside the buttons on the remote indexed corresponding 
                   "CH-\0", "CH \0", "CH+\0",                                       // to the buttons on the remote starting at button 0 in the top left (CH-)
                   "|<<\0", ">>|\0", ">||\0",                                       // and reading from left to right to the bottom right button (9) 
                   " - \0", " + \0", "EQ \0",                                   
                   " 0 \0", "100\0", "200\0",                                       // To be displayed on the LCD
                   " 1 \0", " 2 \0", " 3 \0", 
                   " 4 \0", " 5 \0", " 6 \0", 
                   " 7 \0", " 8 \0", " 9 \0"
                  };
int color[21]={                                                                     // Array containing variable name for the color of the buttons on the remote indexed corresponding 
               RD, RD, RD,                                                          // to the buttons on the remote starting at button 0 in the top left (CH-)
               BU, BU, GR,                                                          // and reading from left to right to the bottom right button (9)
               MA, MA, MA,
               BK, BK, BK,                                                          // Used for LCD display
               BK, BK, BK,
               BK, BK, BK,
               BK, BK, BK
              };

    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char RTC_ALARM_Txt[]= "0";                      //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_text[]   = "075F";


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0F;
    TRISA = 0x00;
    TRISB = 0x01;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;

    RBPU=0;
    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    init_INTERRUPT();
    FAN = 0;
}

void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  

//    FAN_EN = 1;
//    FAN_LED = 1;      //optional on RE2 (D3 Blue)
//    FAN_PWM = 1;
//    duty_cycle = 100;
//    while (1)
//    {
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {   
//            rpm = get_RPM();
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }
//    }
    
//    FAN_EN = 1;
//    duty_cycle = 50;
//    do_update_pwm(duty_cycle);
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            rpm = get_RPM();
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }    
//    
    FAN_EN = 1;
    duty_cycle = 50;    // change in part 2 0-100 to change fan speed
    do_update_pwm(duty_cycle);
//    while (1)
//    {
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            rpm = get_RPM();
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }
//    }
//    
    while (1)
    {

        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            DS1621_tempC = DS1621_Read_Temp();
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
            rpm = get_RPM();
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            Set_RPM_RGB(rpm);
            Set_DC_RGB(duty_cycle);
            Update_Screen();
        }
        
       if (nec_ok == 1)
        {
            nec_ok = 0;

            printf ("NEC_Code = %x\r\n", Nec_code1);       // make sure to use Nec_code1

            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_code1 == array1[j]) 
                {
                    found = j;
                    j = 21;
                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
            }
            else
            {
                Do_Beep();
                printf ("button = %d \r\n", found);
                // add code to process buttons
                if (found == 5)                                                        // if the button pressed on the remote is ">||" (code 0x90) (play pause)
                {
                   Toggle_Fan();                                                // reset clock to setup time 
                }
                
                if (found == 6)                                                        // if the button pressed on the remote is "EQ" (code 0x90)
                {
                   Decrease_Speed();                                                // reset clock to setup time 
                }
                
                if (found == 7)                                                        // if the button pressed on the remote is "EQ" (code 0x90)
                {
                   Increase_Speed();                                                // reset clock to setup time 
                }
                if (found == 8)                                                        // if the button pressed on the remote is "EQ" (code 0x90)
                {
                   DS3231_Setup_Time();                                                // reset clock to setup time 
                }

                PORTD = ((PORTD & 0x07) | D3[found]) & 0xFF;
            }            
        }
    }
}







