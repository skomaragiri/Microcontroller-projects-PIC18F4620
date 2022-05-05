#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#include "ST7735.h"
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz

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

#define SEC_LED      PORTBbits.RB7

void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();
void waitHalfSecond();
void Activate_Buzzer();
void Deactivate_Buzzer();

unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char array1[21]=                                    // IR codes
{
    0xA2,0x62,0xE2,
    
    0x22,0x02,0xC2,
    
    0xE0,0xA8,0x90,
    
    0x68,0x98,0xB0,
    
    0x30,0x18,0x7A,
    
    0x10,0x38,0x5A,
    
    0x42,0x4A,0x52,
};

char txt1[21][4] ={
    "CH-\0", "CH\0", "CH+\0", 
    
    "|<<\0",">>|\0",">||\0",
    
    " - \0"," + \0","EQ \0",
    
    " 0 \0","100\0","200\0",
    
    " 1 \0"," 2 \0"," 3 \0",
    
    " 4 \0"," 5 \0"," 6 \0", 
    
    " 7 \0"," 8 \0"," 9 \0"};

int color[21]=                                      // Color of LCD circle that is displayed corresponding to button press
{
    RD,RD,RD,
    BU,BU,GR,
    MA,MA,MA,
    BK,BK,BK,
    BK,BK,BK,
    BK,BK,BK,
    BK,BK,BK,
};

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

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCONbits.INT0IF == 1) INT0_isr();
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void INT0_isr(void)
{
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            // Clear Timer 1
            TMR1H = 0;                      // Reset Timer1
            TMR1L = 0;                      //
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High            
            return;
        }
        
        case 1 :
        {
          if(Time_Elapsed >= 8500 && Time_Elapsed <= 9500)
          {
              Nec_state = 2;
          } 
          else 
          {
              force_nec_state0 ();
          }
          INTCON2bits.INTEDG0 = 0;      //Change Edge interrupt of INT0 to High low
          return;
        }
        
        case 2 :                            // Add your code here
        {
          if(Time_Elapsed >= 4000 && Time_Elapsed <= 5000)
          {
              Nec_state = 3;
          } 
          else 
          {
              force_nec_state0 ();
          }
          INTCON2bits.INTEDG0 = 1;      //Change Edge interrupt of INT0 to High low
          return;
        }
        
        case 3 :                            // Add your code here
        {
          if(Time_Elapsed >= 400 && Time_Elapsed <= 700)
          {
            Nec_state = 4;
          }
          else 
          {
            force_nec_state0();
          }
          INTCON2bits.INTEDG0 = 0;      //Change Edge interrupt of INT0 to High low
          return;
        }
        
        case 4 :                            // Add your code here
        {
          if(Time_Elapsed >= 400 && Time_Elapsed <= 1800)
          {
                Nec_code = Nec_code << 1;
                if(Time_Elapsed > 1000)
                {
                    Nec_code += 1;
                }
                bit_count++;
                if(bit_count > 31)
                {
                    nec_ok = 1;
                    INTCONbits.INT0IE = 0;
                    Nec_state = 0;
                }
                Nec_state = 3;
          } 
          else 
          {
              force_nec_state0 ();
          }
          INTCON2bits.INTEDG0 = 1;      //Change Edge interrupt of INT0 to High low
          return;
        }
    }
}

void main()
{
    init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0;                              // Enable PORTB internal pull up resistor
    TRISB = 0x01;
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;
    TRISE = 0x00;      
    PORTD = 0x00;       
    PORTE = 0x00; 
    ADCON1 = 0x0F;                          //
    Initialize_Screen();
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                  // Enable external interrupt
    TMR1H = 0;                              // Reset Timer1
    TMR1L = 0;                              //
    PIR1bits.TMR1IF = 0;                    // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                    // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                    // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                     // Enable global interrupts
    nec_ok = 0;                             // Clear flag
    Nec_code = 0x0;                         // Clear code
    
    while(1)
    {
        if (nec_ok == 1)
        {
            nec_ok = 0;

            Nec_code1 = (char) ((Nec_code >> 8));
            printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            
            char found = 0xff;
            
            // add code here to look for code
            for( int i = 0; i < 21; i++ ) 
            {
                if( Nec_code1 == array1[i] )        // Check the array for the transimission 
                {
                    found = i;                      // Since all arrays are mapped in the same way, passing the index will give you the correct value for found
                } 
                
            }
            
            
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
                PORTD = D1[found] | D3[found];                                                      // Turn LEDs to color Start
                PORTE = D2[found];    
                Activate_Buzzer();   // Turn LEDs to color Start
                SEC_LED = 1;
                waitHalfSecond();  
                Deactivate_Buzzer();// Turn on buzzer and activation (SEC) LED for roughly 500ms
                SEC_LED = 0;
            }
        }
    }
}


void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Sp 22-S1");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 10 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}


void Activate_Buzzer()                                                                             // Function used to turn buzzer on
{
 PR2 = 0b11111001 ;
 T2CON = 0b00000101 ;
 CCPR2L = 0b01001010 ;
 CCP2CON = 0b00111100 ;
}

void Deactivate_Buzzer()                                                                           // Function used to turn buzzer off
{
 CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
}

        
void waitHalfSecond()
{
    T0CON = 0x03;                                                                                     // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                                                     // set the lower byte of TMR
    TMR0H = 0x0B;                                                                                     // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                                            // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1; // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                                                   // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;// turn off the Timer 0
}