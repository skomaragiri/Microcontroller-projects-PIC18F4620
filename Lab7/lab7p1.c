#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#pragma config CCP2MX = PORTBE 

#define MODE_LED PORTBbits.RB7
#define SEC_LED PORTDbits.RD7


#define NS_RED PORTAbits.RA5
#define NS_GREEN PORTBbits.RB0 
#define NSLT_RED PORTBbits.RB1
#define NSLT_GREEN PORTBbits.RB2 
#define EW_RED PORTBbits.RB5
#define EW_GREEN PORTBbits.RB6 
#define EWLT_RED PORTEbits.RE1
#define EWLT_GREEN PORTEbits.RE2 

//switches
#define NSLT_SW PORTAbits.RA1                                                   //Definitions to simplify coding
#define EWLT_SW PORTAbits.RA2
#define NSPED_SW PORTAbits.RA3
#define EWPED_SW PORTAbits.RA4
#define SEG_G PORTEbits.RE0

#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3

char arr[10] = {0xFF, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18};
//char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10}; //array using hex to set the correct bits of ports so as to display the appropriate digits on the 7seg 
//char array[10] = {0x7F, 0x4F, 0x12, 0x06, 0x4C, 0x24, 0x20, 0x0f, 0x00, 0x04};

void putch(char c)
{
    while(!TRMT);
    TXREG = c;
}

unsigned int Get_Full_ADC(void)
{
    int result;                                                                 // Where the result is stored
    ADCON0bits.GO = 1;                                                          //Start Conversion
    while(ADCON0bits.DONE==1);                                                  //Wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                                         //Combine result and upper byte and lower byte
                                                                                //Into the result
    return result;                                                              //Return the result
}

//DOUBLE CHECK VALUES
void Init_ADC(void)
{
    ADCON1 = 0x0E; 
    ADCON2 = 0xA9;
}

void init_UART()                                                                //Code to Initalize UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
        USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
        USART_BRGH_HIGH, 25);
    OSCCON = 0x60;                                                              //clock
}

void Wait_Half_Second()
{
 T0CON = 0x02; // Timer 0, 16-bit mode, prescaler 1:8
 TMR0L = 0xDB; // set the lower byte of TMR
 TMR0H = 0x0B; // set the upper byte of TMR
 INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
 T0CONbits.TMR0ON = 1; // Turn on the Timer 0
 while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
 T0CONbits.TMR0ON = 0; // turn off the Timer 0
}

void Wait_One_Second()
{
 SEC_LED = 0; // First, turn off the SEC LED
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 1; // then turn on the SEC LED
 Wait_Half_Second(); // Wait for half second (or 500 msec)
}
 

void Wait_N_Seconds (char seconds)
{
char I;
 for (I = 0; I< seconds; I++)
 {
    Wait_One_Second();
 }
}

void SEVEN_SEG_OFF()
{
    PORTC = 0x7F;
    PORTD = 0x7F;
    PORTE = 0x01;
}

void Set_NS(char color) 
{ 
 switch (color) 
 { 
 case OFF: NS_RED =0;NS_GREEN=0;break; // Turns off the NS LED 
 case RED: NS_RED =1;NS_GREEN=0;break; // Sets NS LED RED 
 case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN 
 case YELLOW: NS_RED =1;NS_GREEN=1;break; // sets NS LED YELLOW 
 } 
} 
void Set_NSLT(char color) 
{ 
 switch (color) 
 { 
 case OFF: NSLT_RED =0;NSLT_GREEN=0;break; // Turns off the NS LED 
 case RED: NSLT_RED =1;NSLT_GREEN=0;break; // Sets NS LED RED 
 case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NS LED GREEN 
 case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break; // sets NS LED YELLOW 
 } 
} 
void Set_EW(char color) 
{ 
 switch (color) 
 { 
 case OFF: EW_RED =0;EW_GREEN=0;break; // Turns off the NS LED 
 case RED: EW_RED =1;EW_GREEN=0;break; // Sets NS LED RED 
 case GREEN: EW_RED =0;EW_GREEN=1;break; // sets NS LED GREEN 
 case YELLOW: EW_RED =1;EW_GREEN=1;break; // sets NS LED YELLOW 
 } 
} 
void Set_EWLT(char color) 
{ 
 switch (color) 
 { 
 case OFF: EWLT_RED =0;EWLT_GREEN=0;break; // Turns off the EWLT LED 
 case RED: EWLT_RED =1;EWLT_GREEN=0;break; // Sets EWLT LED RED 
 case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets EWLT LED GREEN 
 case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break; // sets EWLT LED YELLOW 
 } 
} 
void Activate_Buzzer()
{
 PR2 = 0b11111001 ;
 T2CON = 0b00000101 ;
 CCPR2L = 0b01001010 ;
 CCP2CON = 0b00111100 ;
} 

void Deactivate_Buzzer()
{
 CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
}

void Wait_One_Second_With_Beep()
{
 SEC_LED = 1; // First, turn on the SEC LED
 Activate_Buzzer(); // Activate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 0; // then turn off the SEC LED
 Deactivate_Buzzer (); // Deactivate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
} 

void Display_Lower(char d) 
{
//    PORTD = arr[d];                                                             //Set PORTD to the number you want to show on 7-seg display
    PORTD = (PORTD * 0x80) | arr[d];
}

void Display_Upper(char d) 
{
    PORTC = arr[d] & 0x3F; 
    if((arr[d] & 0x40)==0x40 )SEG_G=1; 
    else SEG_G=0; 
}

void PED_Control( char Direction, char Num_Sec) {
    Display_Upper(0x00);
    Display_Lower(0x00); 
    for(char i = Num_Sec-1; i>0; i--){
        if (Direction ==0)
        {
            Display_Upper(i);
        }
        else 
        {
            Display_Lower(i);
        }
        Wait_One_Second_With_Beep(); 
    }
    Display_Lower(0x00); 
    Display_Upper(0x00); 
    Wait_One_Second_With_Beep(); 
        
    }

void Night_Mode(){
    SEVEN_SEG_OFF();
    Set_NS(GREEN);
    Set_NSLT(RED);              // Step 1
    Set_EW(RED);
    Set_EWLT(RED);
    
    
    Wait_N_Seconds(8);          // Step 2   
    
    Set_NS(YELLOW);             // Step 3  
    Wait_N_Seconds(3);
    
    Set_NS(RED);                // Step 4
    
    if(EWLT_SW == 1)            // Step 5 
    {
        Set_EWLT(GREEN);        // Step 6 
        Wait_N_Seconds(6);
        
        Set_EWLT(YELLOW);       // Step 7
        Wait_N_Seconds(3);
        
        Set_EWLT(RED);         // Step 8
    }
    
    Set_EW(GREEN);             // Step 9 
    Wait_N_Seconds(8);
    
    Set_EW(YELLOW);            // Step 10
    Wait_N_Seconds(3);
    
    Set_EW(RED);               // Step 11
    
    if (NSLT_SW==1)            // Step 12
    {
        Set_NSLT(GREEN);       // Step 13
        Wait_N_Seconds(6);
    
        Set_NSLT(YELLOW);      // Step 14
        Wait_N_Seconds(3);
    
        Set_NSLT(RED);         // Step 15    
    }
}
void Day_Mode()
{
    Set_NS(GREEN); 
    Set_NSLT(RED); 
    Set_EWLT(RED); 
    Set_EW(RED); 
    
    if(NSPED_SW == 1)
    {
        PED_Control(0, 6);
    }
    Wait_N_Seconds(9);
    Set_NS(YELLOW); 
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EWLT_SW == 1)
    {
        Set_EWLT(GREEN);
        Wait_N_Seconds(6);
        Set_EWLT(YELLOW);
        Wait_N_Seconds(3);
        Set_EWLT(RED);
    }
    Set_EW(GREEN);
    if(EWPED_SW == 1)
    {
        PED_Control(1, 9);
    }
    Wait_N_Seconds(7);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED);
    if(NSLT_SW == 1)
    {
        Set_NSLT(GREEN);
        Wait_N_Seconds(6);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}
//void Day_Mode()
//{
//    SEVEN_SEG_OFF();
//    Set_NS(GREEN);              // Step 1 and Step 2
//    Set_NSLT(RED);              
//    Set_EW(RED);
//    Set_EWLT(RED);
//    
//    if(NSPED_SW==1)
//    {
//        PED_Control(0,6); 
//    }
//    Wait_N_Seconds(9);
//    
//    Set_NS(YELLOW);             // Step 3
//    Wait_N_Seconds(3);
//    
//    Set_NS(RED);                // Step 4
//    
//    if(EWLT_SW==1)              // Step 5
//    {
//        Set_EWLT(GREEN);        // Step 6  
//        Wait_N_Seconds(6);
//        
//        Set_EWLT(YELLOW);       // Step 7
//        Wait_N_Seconds(3);
//        
//        Set_EWLT(RED);          // Step 8
//    }
//    
//    Set_EW(GREEN);              // Step 9
//    
//    if(EWPED_SW==1)
//    {
//        PED_Control(1,9);       // Count from 8 to 0
//    }
//        Wait_N_Seconds(7);      // Step 10
//        
//        Set_EW(YELLOW);         // Step 11
//        Wait_N_Seconds(3);
//        
//        Set_EW(RED);            // Step 12 
//    
//    if(NSLT_SW==1)             // Step 13
//    {
//        Set_NSLT(GREEN);       // Step 14
//        Wait_N_Seconds(6);
//        
//        Set_NSLT(YELLOW);      // Step 15
//        Wait_N_Seconds(3); 
//        
//        Set_NSLT(RED);        // Step 16
//    }
//}


void main()
{
   init_UART();
   Init_ADC(); 
   TRISA = 0x1F;        //TRISA is used to set the pins of port A as I/O (1 = input, 0 = output) so 0xff = 1111 1111 making all pins of port A an input
   TRISB = 0x00;        //TRISB is used to set the pins of port B as I/O (1 = input, 0 = output) so 0x00 = 0000 0000 making all pins of port B an output
   TRISC = 0x00;        //TRISC is used to set the pins of port C as I/O (1 = input, 0 = output) so 0 = 0000 0000 making all pins of port C an output
   TRISD = 0x00;        //sets the data read from port C to be 0 as we are not using it in this part
   TRISE = 0x00;        // Make PORTE as all outputs
                        //ADCON1 is an analog to digital converter for PORTA where setting a pin to 1 converts analog voltage signals to a usable digital signal
   
    while (1)
    {
        int num_step = Get_Full_ADC();
        int voltage = num_step * 4; 
        printf ("Light Volt = %d mV \r\n\n", voltage);
        
        if(voltage < 2500)
        {
            MODE_LED = 1;
            Day_Mode();
        } 
        else 
        {
            MODE_LED = 0;
            Night_Mode();
        }
    } 
} 
