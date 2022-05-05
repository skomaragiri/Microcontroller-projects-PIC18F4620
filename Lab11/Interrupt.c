#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"

unsigned char Nec_state = 0;
unsigned char i,bit_count;
extern short nec_ok = 0;
unsigned long long Nec_code;
unsigned int Time_Elapsed;

extern char Nec_code1;

void init_INTERRUPT()
{
    INTCONbits.INT0IF = 0;                                                      // Clear external interrupt flag
    INTCON2bits.INTEDG0 = 0;                                                    // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                                                      // Enable external interrupt
    TMR1H = 0;                                                                  // Reset Timer1
    TMR1L = 0;                                                                  //
    PIR1bits.TMR1IF = 0;                                                        // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                                        // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                                        // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                                         // Enable global interrupts
}

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();                                     // if Timer 1 overflow interrupt occurs call the Timer 1 ISR
    if (INTCONbits.INT0IF == 1) INT0_ISR();                                     // if INT0 external interrupt occurs call the INT0 ISR
}

void TIMER1_isr(void)
{
    Nec_state = 0;                                                              // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                                                    // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                                                       // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                                                        // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;                                                                // Reset decoding process
    T1CONbits.TMR1ON = 0;                                                       // turn OFF TMR1
}

void INT0_ISR() 
{    
    INTCONbits.INT0IF = 0;                                                      // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;                                    // Store Timer1 value
        TMR1H = 0;                                                              // Reset Timer1
        TMR1L = 0;                                                              //
    }
    
    switch(Nec_state)                                                           // Case statements for decoding IR pulses
    {
        case 0 :
        {
                                                                                // Clear Timer 1
            TMR1H = 0;                                                          // Reset Timer1
            TMR1L = 0;                                                          //
            PIR1bits.TMR1IF = 0;                                                // Clear TMR1 interrupt flag
            T1CON= 0x90;                                                        // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;                                               // Enable Timer 1
            bit_count = 0;                                                      // Force bit count (bit_count) to 0
            Nec_code = 0;                                                       // Set Nec_code = 0
            Nec_state = 1;                                                      // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;                                            // Change Edge interrupt of INT0 to Low to High (rising edge)           
            return;
        }
        
        case 1 :
        {
            if((Time_Elapsed > 8500) && (Time_Elapsed < 9500))                  // if pulse is approximately 9ms long advance decoding process (checking for leading pulse)
            {
                Nec_state = 2;                                                  
            }
            else force_nec_state0();                                            // otherwise restart decoding
            
            INTCON2bits.INTEDG0 = 0;                                            // Change Edge interrupt of INT0 to High to Low (falling edge)
            return;
                
        }
        
        case 2 :                            
        {
            if((Time_Elapsed > 4000) && (Time_Elapsed < 5000))                  // if pulse is approximately 4.5ms long advance decoding process 
            {                                                                   // (checking for space following the leading pulse)
                Nec_state = 3;
                  
            }
            else force_nec_state0();                                            // else restart decoding process
            
            INTCON2bits.INTEDG0 = 1;                                            // Change Edge interrupt of INT0 to LOw to High (rising edge) 
            return;            

        }
        
        case 3 :                            
        {
            if((Time_Elapsed > 400) && (Time_Elapsed < 700))                    // if pulse is approximately 562us long advance decoding process (short logic high)     
            {
                Nec_state = 4;
                
            }
            else force_nec_state0();                                            // else restart decoding process
            
            INTCON2bits.INTEDG0 = 0;                                            // Change Edge interrupt of INT0 to High to Low (falling edge)   
            return;
        }
        
        case 4 :                            
        {
            if((Time_Elapsed > 400) && (Time_Elapsed < 1800))                   // if pulse is between 400us and 1800us (1.8ms) long [checking logic 0 or 1] 
            {
                Nec_code = Nec_code << 1;                                       // right shift Nec_code 1 bit
                if(Time_Elapsed > 1000) Nec_code++;                             // if pulse is long (greater than 1ms) increment Nec_code
                bit_count++;                                                    // increment bit count (move to next bit of data received from IR signal)
                if(bit_count > 31)                                              // check if all 32 bits of full message decoded
                {
                    nec_ok = 1;
                    Nec_code1 = (char) ((Nec_code >> 8));// set decoding complete flag
                    INTCONbits.INT0IE = 0;                                      // disable external interrupt INT0
                    Nec_state = 0;                                              // restart decoding process for next signal
                }
                Nec_state = 3;                                                  // return to step 3 of decoding 
            }
            else force_nec_state0();                                            // if pulse is NOT between 400us and 1800us (1.8ms) long restart decoding process
            
            INTCON2bits.INTEDG0 = 1;                                            // Change Edge interrupt of INT0 to Low to High (rising edge)   
                     
            return;
        }
    }                              
} 



