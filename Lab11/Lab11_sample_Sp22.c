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
#include "ST7735.h"
short nec_ok;

signed char tempC;
signed char tempF;

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define KEY_PRESSED PORTBbits.RB7                                               // Location of KEY_PRESSED LED


// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define kp               PORTEbits.RE1     

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

//#define RD               ST7735_RED
//#define BU               ST7735_BLUE
//#define GR               ST7735_GREEN
//#define MA               ST7735_MAGENTA
//#define BK               ST7735_BLACK

char tempSecond = 0xff; 
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

#define DR1             0x01                        // PORT D Start
#define DG1             0x02
#define DB1             0x04
#define DM1             0x05                        
#define DW1             0x07                        // PORT D End

#define DR2             0x01                        // PORT E Start
#define DG2             0x02
#define DB2             0x04                        
#define DM2             0x05
#define DW2             0x07                        // PORT E End

#define DR3             0x10                        // PORT D Start
#define DG3             0x20
#define DB3             0x40
#define DM3             0x50  
#define DW3             0x70                        // PORT D End

char D1[21] =                                       // Color Defintions Start
{
    0x00,0x00,DR1,
    0x00,0x00,DG1,
    0x00,0x00,DM1,
    0x00,0x00,DW1,
    0x00,0x00,DW1,
    0x00,0x00,DW1,
    0x00,0x00,DW1,
};
char D2[21] = 
{
    0x00,DR2,0x00,
    0x00,DB2,0x00,
    0x00,DM2,0x00,
    0x00,DW2,0x00,
    0x00,DW2,0x00,
    0x00,DW2,0x00,
    0x00,DW2,0x00,
};
char D3[21] = 
{
    DR3,0x00,0x00,
    DB3,0x00,0x00,
    DM3,0x00,0x00,
    DW3,0x00,0x00,
    DW3,0x00,0x00,
    DW3,0x00,0x00,
    DW3,0x00,0x00,
};

char array1[21]={                                                               // Array containing the hex data byte gathered from Tera Term indexed corresponding
                 0xa2, 0x62, 0xe2,                                              // to the buttons on the remote starting at button 0 in the top left (CH-)
                 0x22, 0x02, 0xc2,                                              // and reading from left to right to the bottom right button (9) 
                 0xe0, 0xa8, 0x90,
                 0x68, 0x98, 0xb0,
                 0x30, 0x18, 0x7a,
                 0x10, 0x38, 0x5a,
                 0x42, 0x4a, 0x52
                };
char txt1[21][4] ={                                                             // Array containing the text inside the buttons on the remote indexed corresponding 
                   "CH-\0", "CH \0", "CH+\0",                                   // to the buttons on the remote starting at button 0 in the top left (CH-)
                   "|<<\0", ">>|\0", ">||\0",                                   // and reading from left to right to the bottom right button (9) 
                   " - \0", " + \0", "EQ \0",                                   
                   " 0 \0", "100\0", "200\0",                                   // To be displayed on the LCD
                   " 1 \0", " 2 \0", " 3 \0", 
                   " 4 \0", " 5 \0", " 6 \0", 
                   " 7 \0", " 8 \0", " 9 \0"
                  };
int color[21]={                                                                 // Array containing variable name for the color of the buttons on the remote indexed corresponding 
               RD, RD, RD,                                                      // to the buttons on the remote starting at button 0 in the top left (CH-)
               BU, BU, GR,                                                      // and reading from left to right to the bottom right button (9)
               MA, MA, MA,
               BK, BK, BK,                                                      // Used for LCD display
               BK, BK, BK,
               BK, BK, BK,
               BK, BK, BK
              };

char TempSecond;

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char Nec_code1;
char found;

//short nec_ok = 0;
//unsigned long Nec_code;

void Deactivate_Buzzer();
void Activate_Buzzer();
void Initialize_Screen();
void Wait_One_Sec();

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & 
               USART_RX_INT_OFF & 
               USART_ASYNCH_MODE & 
               USART_EIGHT_BIT & 
               USART_CONT_RX & 
               USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Wait_Half_Second()
{
    T0CON = 0x03;                                                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                               // set the lower byte of TMR
    TMR0H = 0x0B;                                                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                       // turn off the Timer 0
}


void Wait_One_Sec()
{
    Wait_Half_Second();
    Wait_Half_Second();
}

void Wait_One_Second_With_Beep()                                                // creates one second delay as well as sound buzzer
{

 KEY_PRESSED = 1;                                                               // First, turn on the KEY_PRESSED LED
 strcpy(txt,"*");                                                               // Write '*'                                            
 drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                           // At location (120, 10), Text Color = WHITE, background = BLACK, Text Size = 1
 Activate_Buzzer();                                                             // Activate the buzzer
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec)
 
 KEY_PRESSED = 0;                                                               // then turn off the KEY_PRESSED LED
 strcpy(txt," ");                                                               // Write ' ' (get rid of *)
 drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                           // At location (120, 10), Text Color = WHITE, background = BLACK, Text Size = 1
 Deactivate_Buzzer ();                                                          // Deactivate the buzzer
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec)
}


void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    
    init_INTERRUPT();
    //DS1621_Init();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1= 0x0F;		    // Fill out values
    TRISA = 0x00;
    TRISB = 0x11;
    TRISC = 0x00;                   
    TRISD = 0x80;
    TRISE = 0x00;
    RBPU=0;
    I2C_Init(100000); 

    DS1621_Init();

}


void main() 
{ 
    Do_Init();                                                  // Initialization  
    Initialize_Screen();
//    while (1)							// This is for the DS1621 testing. Comment this section
//    {								// out to move on to the next section
//        signed char tempC = DS1621_Read_Temp();
//        signed char tempF = (tempC * 9 / 5) + 32;
//        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
//        Wait_One_Sec();
//    }
    /////// END PART 1 ///////
    
      DS3231_Setup_Time();    
      while (1)						// This is for the DS3231 testing. Comment this section
      {							// out to move on to the next section

        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();
            signed char tempF = (tempC * 9 / 5) + 32;
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
        }
    
      //}// delete later
//}// delete later
   /////////// END PART 2 ///////////////
    
        if (nec_ok == 1)                                                        // if decoding complete
        {
            nec_ok = 0;                                                         // clear decoding complete flag to allow for another decoding process to happen

                                           // left shift the IR Nec_code by 8 bits to isolate the data bits
            
            INTCONbits.INT0IE = 1;                                              // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;                                            // Edge programming for INT0 falling edge
            
            char found = 0xff;                                                  // variable found default as decimal 255
            
            
            
            for(int i = 0; i < 21; i++)                                         // loop to identify location of remote button using previously defined arrays
            {
                if(Nec_code1 == array1[i])                                      // if Nec_code data bits match the code in the array that indexes the Nec_code data byte hex value  
                {                                                               // with the location of the remote buttons
                    found = i;                                                  // assign variable found with that index (location) 
                }
                    
            }
            
            printf ("NEC_Code = %x\t %d\r\n", Nec_code1,found);  // Print full Nec_code as well as seperate and print Nec_code data byte 
                                                                                // and print the button location number 
            if (found != 0xff)                                                  // if found is not default value (a valid key was pressed) display on LCD
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]);      
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);
                PORTD = D1[found] | D3[found];                                                      // Turn LEDs to color Start
                PORTE = D2[found];
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, 
                           ST7735_BLACK,TS_1); 
            }
             if (found == 8) 
             {
                DS3231_Setup_Time(); 
             }
    
                Wait_One_Second_With_Beep();                                        // With every key press flash KEY_PRESSED LED and sound buzzer over 1sec
        }
      }

}
    
    
    //} //put back
//}



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

void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L SP 22-S1");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 11 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}




