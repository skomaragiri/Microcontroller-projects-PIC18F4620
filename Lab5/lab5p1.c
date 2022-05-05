#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define D1_RED PORTBbits.RB5                                 //Definitions to simplify coding
#define D1_GREEN PORTBbits.RB6
#define D1_BLUE PORTBbits.RB7

#define D2_RED PORTBbits.RB0                                 //Definitions to simplify coding
#define D2_GREEN PORTBbits.RB1
#define D2_BLUE PORTBbits.RB2

#define D3_RED PORTAbits.RA4                                 //Definitions to simplify coding
#define D3_GREEN PORTAbits.RA5

//void D1_OFF(void);
//void D1_RED(void);
//void D1_G(void);
//void D1_OFF(void);

#define delay 5
// Prototype Area to place all the references to the routines used in the program

void Init_ADC(void);

char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10}; // Index of array is the number that will be displayed 

void Select_ADC_Channel(char channel) // Function to select analog port for ADC conversion 
{
    ADCON0 = channel * 4 + 1;
}

void Init_ADC(void) 
{
 ADCON1=0x1B; // Select pins AN0 through AN3 as analog signal, VDD-VSS as
              // Reference voltage
 ADCON2=0xA9; // Right justify the result. Set the bit conversion time (TAD) and
              // Acquisition time
}
unsigned int Get_Full_ADC(void)
{
int result;
 ADCON0bits.GO=1;                        // Start Conversion
 while(ADCON0bits.DONE==1);             // Wait for conversion to be completed (DONE=0)
 result = (ADRESH * 0x100) + ADRESL;    // Combine result of upper byte and lower byte into
 return result;                          // return the most significant 8- bits of the result.
} 

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}

void init_UART()                                        //Code to Initialize UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
        USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
        USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}
//Setting output for RGB LED D1
void SET_D1_OFF(void)               //000                           
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0;
}

void SET_D1_RED(void)                  //001
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
}
void SET_D1_GREEN(void)            //010
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_YELLOW(void)            //011
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_BLUE(void)  //100
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}
void SET_D1_PURPLE(void)            //101
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 1;
}
void SET_D1_CYAN(void)            //110
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_WHITE(void)         //111
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 1;
}



//Setting output for RGB LED D2
void SET_D2_OFF(void)               //000                     
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0;
}

void SET_D2_RED(void)                  //001
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void SET_D2_GREEN(void)            //010
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_YELLOW(void)            //011
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_BLUE(void)  //100
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_PURPLE(void)            //101
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_CYAN(void)            //110
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_WHITE(void)         //111
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}


 //Setting output for RGB LED D3
void SET_D3_OFF(void)               //000                          
{
    D3_RED = 0;
    D3_GREEN = 0;

}

void SET_D3_RED(void)                  //001
{
    D3_RED = 1;
    D3_GREEN = 0;

}
void SET_D3_GREEN(void)            //010
{
    D3_RED = 0;
    D3_GREEN = 1;

}
void SET_D3_YELLOW(void)            //011
{
    D3_RED = 1;
    D3_GREEN = 1;
}

void WAIT_1_SEC()
{
    for(int j = 0; j < 17000; j++);
}

void Display_Lower_Digit(char digit)
{
    PORTD = array[digit];
}

void Display_Upper_Digit(char digit)
{
    PORTC = array[digit] & 0x3F;
    char PrtE = array[digit] & 0x40; 
    if (PrtE == 0) PORTE = 0x00; 
    else PORTE = 0x01; 
}

void DISPLAY_D1(int T)//Cases for D1 RGB LED
{
    
    T < 45 ? SET_D1_OFF() : T >= 46 && T < 56 ? SET_D1_RED() : T >= 56 && T < 66 ? SET_D1_GREEN() : T >= 66 && T < 76 ? SET_D1_BLUE() : SET_D1_WHITE();
//    if(T < 45) SET_D1_OFF(); 
//    else if (T >= 46 && T < 56) SET_D1_RED(); 
//    else if (T >= 56 && T < 66) SET_D1_GREEN(); 
//    else if (T >= 66 && T < 76) SET_D1_BLUE(); 
//    else if (T >= 76) SET_D1_WHITE(); 
}

void DISPLAY_D2(int T)//Cases for D2 RGB LED
{
    if (T > 70)                                
        SET_D2_WHITE();
    else
        PORTB = T/10;    
}

void DISPLAY_D3(int mv)//Cases for D3 RGB LED
{
    mv < 2500 ? PORTA = 0x01<<4 : mv >= 2500 && mv < 3400 ? PORTA = 0x02<<4 : PORTA = 0x03<<4;
//    if (mv < 2500)
//    {
//        PORTA = 0x01<<4;
//    }
//    else if (mv >= 2500 && mv < 3400)
//    {
//        PORTA = 0x02<<4;
//    }
//    else {
//        PORTA = 0x03<<4;
//    }    
}

void main(void)
{
    init_UART();
    Init_ADC();
    TRISA = 0x0F; // Make RA0-RA3 inputs, the rest outputs
    TRISB = 0x00; // Make PORTB as all outputs
    TRISC = 0x00; // Make PORTC as all outputs
    TRISD = 0x00; // Make PORTD as all outputs
    TRISE = 0x00; // Make PORTE as all outputs
    
    
    while(1)
    {
//        for(int i = 0; i < 10; i++)
//        {
//            Display_Lower_Digit(i);
//            Display_Upper_Digit(i);
//            WAIT_1_SEC();
//        }
//        PORTC = 0xFF;
//        PORTD = 0xFF;
//        
//        
//        for(int i = 0; i < 8; i++)
//        {
//            PORTB = i;
//            WAIT_1_SEC();
//        }
//        PORTB = 0x00;
//        
//        for(int i = 0; i < 8; i++)
//        {
//            PORTB = i<<5;
//            WAIT_1_SEC();
//        }
//        PORTB = 0x00;
//        
//        for(int i = 0; i < 4; i++)
//        {
//            PORTA = i<<4;
//            WAIT_1_SEC();
//        }
//        PORTA = 0x00;
        
        Select_ADC_Channel(0); // Select RA0 for analog conversion
        int num_step = Get_Full_ADC(); // Obtain the steps of the ADC conversion
        float voltage_mv = num_step * 4.0;  // Convert the Analog to Digital conversion to a usable voltage measurement
        float temperature_C = (1035.0 - voltage_mv) / 5.50; 
        float temperature_F = (1.80 * temperature_C) + 32.0;
        int tempF = (int) temperature_F;
        char U = tempF / 10; // Extract the Tens Place and initialize it to U
        char L = tempF % 10; // Extract the Ones Place and initialize it to L
        Display_Upper_Digit(U);
        Display_Lower_Digit(L);


        printf ("Steps = %d \r\n", num_step);
        printf ("Voltage = %f \r\n", voltage_mv);
        printf ("Temperature = %d/ F \r\n\n", tempF);
        DISPLAY_D2(tempF);
        DISPLAY_D1(tempF);
          
        Select_ADC_Channel(1);  // Select RA0 for analog conversion 
        int num_step = Get_Full_ADC(); // Obtain the steps of the ADC conversion   
        int voltage_mv1 = num_step * 4; // Convert the Analog to Digital conversion to a usable voltage measurement 
        printf ("Light Volt = %d mV \r\n\n", voltage_mv1);
        DISPLAY_D3(voltage_mv1);
        
        WAIT_1_SEC();
    }
}

