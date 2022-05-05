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

#define debug 1
//LEDS
#define _XTAL_FREQ      8000000                                                                     // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4                                                                // Timer Clock 2 Mhz

#define TFT_DC       PORTCbits.RC0                                                                  // Location of TFT D/C
#define TFT_CS       PORTCbits.RC1                                                                  // Location of TFT Chip Select
#define TFT_RST      PORTCbits.RC2                                                                  // Location of TFT Reset
#define SEC_LED      PORTDbits.RD7                                                                  // Location of Second Count LED



#define NS_RED       PORTDbits.RD2                                                                  //Defines and assigns a name to specified bit for more user friendly application       
#define NS_GREEN     PORTDbits.RD3                                                                  //Defines and assigns a name to specified bit for more user friendly application

#define NSLT_RED     PORTDbits.RD4                                                                  //Defines and assigns a name to specified bit for more user friendly application
#define NSLT_GREEN   PORTDbits.RD5                                                                  //Defines and assigns a name to specified bit for more user friendly application

#define EW_RED       PORTBbits.RB4                                                                  //Defines and assigns a name to specified bit for more user friendly application
#define EW_GREEN     PORTBbits.RB5                                                                  //Defines and assigns a name to specified bit for more user friendly application

#define EWLT_RED     PORTEbits.RE1                                                                  //Defines and assigns a name to specified bit for more user friendly application
#define EWLT_GREEN   PORTEbits.RE2                                                                  //Defines and assigns a name to specified bit for more user friendly application
#define RBPU         INTCON2bits.RBPU

#define NSLT_SW    PORTAbits.RA1                                                                    // Defines and assigns a name to specified bit for more user friendly application
//#define NSPED_SW   PORTAbits.RA3                                                                    // Defines and assigns a name to specified bit for more user friendly application

//#define EWPED_SW   PORTAbits.RA4                                                                    // Defines and assigns a name to specified bit for more user friendly application
#define EWLT_SW    PORTAbits.RA2                                                                    // Defines and assigns a name to specified bit for more user friendly application

#define MODE_LED    PORTBbits.RB7                                                                   // Defines and assigns a name to specified bit for more user friendly application
//colors
#define OFF 		0                                                                               // Defines OFF as decimal value 0
#define RED 		1                                                                               // Defines RED as decimal value 1
#define GREEN 		2                                                                               // Defines GREEN as decimal value 2
#define YELLOW 		3                                                                               // Defines YELLOW as decimal value 3

#define NS          0                                                                               // Defines OFF as decimal value 0
#define NSLT 		1                                                                               // Defines OFF as decimal value 0
#define EW          2                                                                               // Defines OFF as decimal value 0
#define EWLT 		3                                                                               // Defines OFF as decimal value 0

#define Circle_Size     7                                                                           // Size of Circle for Light
#define Circle_Offset   15                                                                          // Location of Circle
#define TS_1            1                                                                           // Size of Normal Text
#define TS_2            2                                                                           // Size of PED Text
#define Count_Offset    10                                                                          // Location of Count

#define XTXT            30                                                                          // X location of Title Text 
#define XRED            40                                                                          // X location of Red Circle
#define XYEL            60                                                                          // X location of Yellow Circle
#define XGRN            80                                                                          // X location of Green Circle
#define XCNT            100                                                                         // X location of Sec Count

#define NS              0                                                                           // Number definition of North/South
#define NSLT            1                                                                           // Number definition of North/South Left Turn
#define EW              2                                                                           // Number definition of East/West
#define EWLT            3                                                                           // Number definition of East/West Left Turn
 
#define Color_Off       0                                                                           // Number definition of Off Color
#define Color_Red       1                                                                           // Number definition of Red Color
#define Color_Green     2                                                                           // Number definition of Green Color
#define Color_Yellow    3                                                                           // Number definition of Yellow Color

#define NS_Txt_Y        20                                                                          // Set location of NS text
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset                                                    // Set location of NS Circles
#define NS_Count_Y      NS_Txt_Y + Count_Offset                                                     // Set location of NS Counter
#define NS_Color        ST7735_BLUE                                                                 // Set color of NS

#define NSLT_Txt_Y      50                                                                          // Set location of NSLT text
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset                                                  // Set location of NSLT Circles
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset                                                   // Set location of NSLT Counter
#define NSLT_Color      ST7735_MAGENTA                                                              // Set color of NSLT

#define EW_Txt_Y        80                                                                          // Set location of NSLT text
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset                                                    // Set location of NSLT Circles
#define EW_Count_Y      EW_Txt_Y + Count_Offset                                                     // Set location of NSLT Counter
#define EW_Color        ST7735_CYAN                                                                 // Set color of NS

#define EWLT_Txt_Y      110                                                                         // Set location of NSLT text
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset                                                  // Set location of NSLT Circles
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset                                                   // Set location of NSLT Counter
#define EWLT_Color      ST7735_WHITE                                                                // Set color of NS

#define PED_NS_Count_Y  30                                                                          // Set Y location of PED_NS Counter text
#define PED_EW_Count_Y  90                                                                          // Set Y location of PED_EW Counter text
#define PED_Count_X      2                                                                          // Set X location of PED counters
#define Switch_Txt_Y   140                                                                          // Set Y location of Switch texts

#include "ST7735_TFT.c"                                                                             // Include file as a library for LCD functions 

char buffer[31];                                                                                    // general buffer for display purpose
char *nbr;                                                                                          // general pointer used for buffer
char *txt;


int INT0_flag, INT1_flag, INT2_flag = 0;

char NS_Count[]      = "00";                                                                        // text storage for NS Count
char NSLT_Count[]    = "00";                                                                        // text storage for NS Left Turn Count
char EW_Count[]      = "00";                                                                        // text storage for EW Count
char EWLT_Count[]    = "00";                                                                        // text storage for EW Left Turn Count

char PED_NS_Count[]  = "00";                                                                        // text storage for NS Pedestrian Count
char PED_EW_Count[]  = "00";                                                                        // text storage for EW Pedestrian Count

char SW_NSPED_Txt[]  = "0";                                                                         // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]   = "0";                                                                         // text storage for NS Left Turn Switch
char SW_EWPED_Txt[]  = "0";                                                                         // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]   = "0";                                                                         // text storage for EW Left Turn Switch

char SW_MODE_Txt[]   = "D";                                                                         // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";                                                                         // text storage for Actual Mode
char FlashingS_Txt[] = "0";                                                                         // text storage for Emergency Status
char FlashingR_Txt[] = "0";                                                                         // text storage for Flashing Request

char dir;
char Count;                                                                                         // RAM variable for Second Count
char PED_Count;                                                                                     // RAM variable for Second Pedestrian Count

char SW_NSPED;                                                                                      // RAM variable for NS Pedestrian Switch
char SW_NSLT;                                                                                       // RAM variable for NS Left Turn Switch
char SW_EWPED;                                                                                      // RAM variable for EW Pedestrian Switch
char SW_EWLT;                                                                                       // RAM variable for EW Left Turn Switch
char SW_NS_PED;
char SW_EW_PED;
char SW_MODE;                                                                                       // RAM variable for Mode Light Sensor
char FLASHING;                                                                  
char FLASHING_REQUEST;
int MODE;                                                                                           // RAM variable for MODE
char direction;                                                                                     // variable for direction input in appropriate functions
float volt;                                                                                         // variable to store result volts after A/D conversion

unsigned int get_full_ADC(void);                                                                    // initialize get_full_ADC function
void Init_ADC(void);                                                                                // initialize Init_ADC function
void init_IO(void);                                                                                 // initialize init_IO function


void Set_NS(char color);                                                                            // initialize set_NS function
void Set_NS_LT(char color);                                                                         // initialize set_NS_LT function
void Set_EW(char color);                                                                            // initialize set_EW function
void Set_EW_LT(char color);                                                                         // initialize set_EW_LT function

void PED_Control( char Direction, char Num_Sec);                                                    // initialize PED_Control function
void Day_Mode(void);                                                                                // initialize Day_Mode function
void Night_Mode(void);                                                                              // initialize Night_Mode function


void Wait_One_Second(void);                                                                         // initialize Wait_One_Second function
void Wait_Half_Second(void);                                                                        // initialize Wait_Half_Second function
void Wait_N_Seconds (char, char);                                                                   // initialize Wait_N_Seconds function
void Wait_One_Second_With_Beep(void);                                                               // initialize Wait_One_Second_With_Beep function

void Initialize_Screen(void);                                                                       // initialize Initialize_Screen function
void update_LCD_color(char , char );                                                                // initialize update_LCD_color function
void update_LCD_Count(char direction, char count);                                                  // initialize update_LCD_count function
void update_LCD_misc(void);                                                                         // initialize update_LCD_misc function
void update_LCD_count(char, char);                                                                  // initialize update_LCD_count function

void interrupt high_priority chkisr();
void INT0_ISR();
void INT1_ISR();
void INT2_ISR();
void Do_Flashing();

void Initialize_Screen()                                                                            // Function to initialize all required info on to the LCD using the functions defined 
{                                                                                                   // in the included library file ST7735_TFT.c
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L SPRING 22-S1");                                                             // Displays class info at the top of the LCD
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                            // Draw text at coordinates (2,2), text color = WHITE, background color = BLACK, Text Size = 1 
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");                                                                             // Displays the current Mode of operation 
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                           // Draw current mode label text at coordinates (2,10), text color = WHITE, background color = BLACK, Text Size = 1
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                  // Draw current mode value text at coordinates (35,10), text color = WHITE, background color = BLACK, Text Size = 1

  /* FLASHING REQUEST FIELD */
  strcpy(txt, "FR:");                                                                               // Displays "FR:"
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                          // At location (50,10) with WHITE text color and a BLACK background, Text Size 1
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                // Write the value of the FR at (70, 10), WHITE text, BLACK background, Text Size 1 
  
  /* FLASHING STATUS FIELD */
  strcpy(txt, "FS:");                                                                               // Display "FS:"
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                          // At location (80,10), WHITE text, BLACK background, Text Size 1
  drawtext(100, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                               // Display value of FS at (100,10), WHITE text, BLACK background, Text Size 1

  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");                                                                                 // Print "*"
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                                         // At location (120,10), WHITE text, BLACK background, Text Size 1
      
  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");                                                                       // Write "NORTH/SOUTH"
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);                                  // At location of NS_Txt_Y defined earlier, Text Color = NS_Color, BLACK background, Text Size 1 
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);                                                   // Draw NS rectangle, color = NS_Color
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);                                            // Draw a RED empty circle inside the NS Rectangle 
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);                                         // Draw a YELLOW empty circle inside the NS Rectangle
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);                                          // Draw a filled GREEN circle inside the NS Rectangle
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);                             // Create counter display for NS direction, background = BLACK, Text Size = 2
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");                                                                            // Write "N/S LT"
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);                                // At location of NSLT_Txt_Y defined earlier, Text Color = NSLT_Color, BLACK background, Text Size 1 
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);                                               // Draw NSLT rectangle, color = NSLT_Color
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);                                             // Draw a filled RED circle inside the NSLT Rectangle 
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);                                          // Draw a YELLOW empty circle inside the NSLT Rectangle
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);                                           // Draw a GREEN empty circle inside the NSLT Rectangle
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);                        // Create counter display for NSLT direction, background = BLACK, Text Size = 2
  
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");                                                                          // Write "EAST/WEST"
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);                                     // At location of EW_Txt_Y defined earlier, Text Color = EW_Color, BLACK background, Text Size 1 
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);                                                    // Draw EW rectangle, color = EW_Color
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);                                               // Draw a filled RED circle inside the EW Rectangle 
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);                                            // Draw a YELLOW empty circle inside the EW Rectangle
    drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);                                           // Draw a GREEN empty circle inside the EW Rectangle
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                              // Create counter display for EW direction, background = BLACK, Text Size = 2

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");                                                                             // Write "E/W LT"
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);                                 // At location of EWLT_Txt_Y defined earlier, Text Color = EWLT_Color, BLACK background, Text Size 1 
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);                                                // Draw EWLT rectangle, color = EWLT_Color
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);                                             // Draw a filled RED circle inside the EWLT Rectangle 
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);                                          // Draw a YELLOW empty circle inside the EWLT Rectangle
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);                                           // Draw a GREEN empty circle inside the EWLT Rectangle
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);                        // Create counter display for EWLT direction, background = BLACK, Text Size = 2

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");                                                                                // Write "PNS"
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);                                          // Draw label text at location of NS_text_Y with the color of NS direction, background = BLACK, Text Size 1
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);                           // Draw value text for the NS PED counter at location of PED_NS_text_Y with the color of NS direction, background = BLACK, Text Size 2
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);                           // Draw value text for the EW PED counter at location of PED_EW_Count_Y with the color of EW direction, background = BLACK, Text Size 2
  strcpy(txt, "PEW");                                                                                // Write "PNS"
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);                                          // Draw label text at location of EW_text_Y with the color of EW direction, background = BLACK, Text Size 1
      
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "NSP NSLT EWP EWLT MR");                                                               // Write "NSP NSLT EWP EWLT MR" at defined locations, text color = WHITE, background = BLACK, Text Size = 1
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void init_UART()                                                                                     // initialize UART code for Tera Term
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;                                                                                  // Set clock frequency
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}



void init_IO()                                                                                      // Function for setting PORT pins as I/O (1 = input, 0 = output) 
{
    TRISA = 0xDF;                                                                                   //
    TRISB = 0x00;                                                                                   // 
    TRISC = 0x00;                                                                                   // 
    TRISD = 0x00;                                                                                   // 
    TRISE = 0x00;                                                                                   // 
}

void Init_ADC()                                                                                     // A/D pin conversion
{
    ADCON0 = 0x01;                                                                                  // enable A/D conversion
    ADCON1= 0x0E;                                                                                   // all PORTA pins as digital I/O except LSB
    ADCON2= 0xA9;                                                                                   // Configure A/D Clock Source
}


void Do_Init()          // Initialize the ports
{
 init_UART();           // Initialize the uart
 Init_ADC();            // Initiailize the ADC with the
                        // programming of ADCON1
 OSCCON=0x70;           // Set oscillator to 8 MHz
 TRISB = 0x07;           // Configure the PORTB to make sure
                                // that all the INTx pins are
 RBPU = 0;
 
    TRISA = 0xDF;                                                                                   //
                                                                                  // 
    TRISC = 0x00;                                                                                   // 
    TRISD = 0x00;                                                                                   // 
    TRISE = 0x00;               // inputs
 INTCONbits.INT0IF = 0;             // Clear INT0IF
 INTCON3bits.INT1IF = 0;            // Clear INT1IF
 INTCON3bits.INT2IF =0;             // Clear INT2IF
 INTCON2bits.INTEDG0=0 ;                // INT0 EDGE falling
 INTCON2bits.INTEDG1=0;             // INT1 EDGE falling
 INTCON2bits.INTEDG2=1;                 // INT2 EDGE rising
 INTCONbits.INT0IE =1;              // Set INT0 IE
 INTCON3bits.INT1IE=1;                  // Set INT1 IE
 INTCON3bits.INT2IE=1;                  // Set INT2 IE
 INTCONbits.GIE=1;                       // Set the Global Interrupt Enable
}



void INT0_ISR()
{
 INTCONbits.INT0IF=0; // Clear the interrupt flag
 if(MODE == 1) 
 {
     SW_NS_PED = 1;
 } // set software INT0_flag
 else 
 {
     SW_NS_PED = 0;
 }
 
}

void INT1_ISR()
{
 INTCON3bits.INT1IF=0; // Clear the interrupt flag
 if(MODE == 1) 
 {
     SW_EW_PED = 1; // set software INT1_flag
 }
 else 
 {
     SW_EW_PED = 0;
 }
 
}

void INT2_ISR()
{
 INTCON3bits.INT2IF=0; // Clear the interrupt flag
 FLASHING_REQUEST = 1; // set software INT2_flag
}


void interrupt high_priority chkisr()
{
 if (INTCONbits.INT0IF == 1) INT0_ISR(); // check if INT0
                                            // has occured
 if (INTCON3bits.INT1IF == 1) INT1_ISR();
 if (INTCON3bits.INT2IF == 1) INT2_ISR();
}

unsigned int get_full_ADC()                                                                         // A/D conversion function
{
unsigned int result;
   ADCON0bits.GO=1;                                                                                 // Start Conversion
   while(ADCON0bits.DONE==1);                                                                       // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                                                              // combine result of upper byte and
                                                                                                    // lower byte into result
   return result;                                                                                   // return the result.
}

void Set_NS(char color)                                                                             // Function for setting NS color
{
    direction = NS;                                                                                 // direction variable gets value of NS previously defined
    update_LCD_color(direction, color);                                                             // Call update_LCD_color function to change traffic light created on LCD
    
    switch (color)                                                      
    {
        case OFF:       NS_RED =0;  NS_GREEN=0; break;                                             // Turns off the NS LED
        case RED:       NS_RED =1;  NS_GREEN=0; break;                                             // Sets NS LED RED
        case GREEN:     NS_RED =0;  NS_GREEN=1; break;                                             // sets NS LED GREEN
        case YELLOW:    NS_RED =1;  NS_GREEN=1; break;                                             // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)                                                                          // Function for setting NSLT color
{
    direction = NSLT;                                                                              // direction variable gets value of NSLT previously defined
    update_LCD_color(direction, color);                                                            // Call update_LCD_color function to change traffic light created on LCD
    
  switch (color)
 {
    case OFF:    NSLT_RED = 0;    NSLT_GREEN=0; break;                                             // Turns off the NSLT LED
    case RED:    NSLT_RED = 1;    NSLT_GREEN=0; break;                                             // Sets NSLT LED RED
    case GREEN:  NSLT_RED = 0;    NSLT_GREEN=1; break;                                             // sets NSLT LED GREEN
    case YELLOW: NSLT_RED = 1;    NSLT_GREEN=1; break;                                             // sets NSLT LED YELLOW
 }
}

void Set_EW(char color)                                                                            // Function for setting EW color
{
    direction = EW;                                                                                // direction variable gets value of EW previously defined
    update_LCD_color(direction, color);                                                            // Call update_LCD_color function to change traffic light created on LCD
    
 switch (color)
 {
    case OFF:    EW_RED = 0;    EW_GREEN=0; break;                                                 // Turns off the EW LED
    case RED:    EW_RED = 1;    EW_GREEN=0; break;                                                 // Sets EW LED RED
    case GREEN:  EW_RED = 0;    EW_GREEN=1; break;                                                 // sets EW LED GREEN
    case YELLOW: EW_RED = 1;    EW_GREEN=1; break;                                                 // sets EW LED YELLOW
 }
}

void Set_EWLT(char color)                                                                          // Function for setting EWLT color
{
    direction = EWLT;                                                                              // direction variable gets value of EWLT previously defined
    update_LCD_color(direction, color);                                                            // Call update_LCD_color function to change traffic light created on LCD
    
 switch (color)
 {
    case OFF:    EWLT_RED = 0;    EWLT_GREEN=0; break;                                             // Turns off the EWLT LED
    case RED:    EWLT_RED = 1;    EWLT_GREEN=0; break;                                             // Sets EWLT LED RED
    case GREEN:  EWLT_RED = 0;    EWLT_GREEN=1; break;                                             // sets EWLT LED GREEN
    case YELLOW: EWLT_RED = 1;    EWLT_GREEN=1; break;                                             // sets EWLT LED YELLOW
 }
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

void update_LCD_PED_Count(char direction, char count)                                              // Function for updating the Pedestrian counter on the LCD
{

   switch (direction)
   {
      case NS:       
        PED_NS_Count[0] = count/10  + '0';                                                         // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';                                                         // PED count Lower digit
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);         // Put counter digit on screen
        break;
      
      // put code here 
        
      case EW:       
        PED_EW_Count[0] = count/10  + '0';                                                         // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';                                                         // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);         // Put counter digit on screen
        break;

   }
   
}

void PED_Control( char direction, char Num_Sec)                                                     // Function for controlling pedestrian displays on LCD
{ 
    for(char i = Num_Sec-1;i>0; i--)
    {
        update_LCD_PED_Count(direction, i);                                                         // updates the pedestrian cross timer on the LCD
        Wait_One_Second_With_Beep();                                                                // 1 second delay with sound of buzzer
    }
    
    update_LCD_PED_Count(direction, 0);                                                             // updates the pedestrian cross timer on the LCD
    Wait_One_Second_With_Beep();
}

void Do_Flashing()
{
    FLASHING = 1;
    while(FLASHING)
    {
        if(FLASHING_REQUEST)
        {
            FLASHING_REQUEST = 0;
            FLASHING = 0;
        }
        else
        {
            Set_NS(RED);                                                                                  
            Set_NSLT(RED);                                                                                  
            Set_EW(RED);                                                                                   
            Set_EWLT(RED);  
            
            Wait_One_Second();
            
            Set_NS(OFF);                                                                                  
            Set_NSLT(OFF);                                                                                  
            Set_EW(OFF);                                                                                   
            Set_EWLT(OFF);  
            
            Wait_One_Second();
            
            
        }
    }
}

void Day_Mode()                                                                                     // Function for day time logic of traffic system
{
    MODE = 1;                                                                                       // MODE variable gets assigned 1, indicates day mode
    MODE_LED = 1;                                                                                   // MODE_LED ON
    Act_Mode_Txt[0] = 'D';                                                                          // LCD displays 'D' for day mode
    
    // put code here 
    Set_NS(GREEN);                                                                                  // Step 1: NS LED = GREEN
    Set_NSLT(RED);                                                                                  //         NSLT LED = RED
    Set_EW(RED);                                                                                    //         EW LED = RED
    Set_EWLT(RED);                                                                                  //         EWLT LED = RED
    
    if(SW_NS_PED == 1)                                                                                 //         check if there is a NS pedestrian (NSPED_SW ON) 
    {
        PED_Control(NS,6);
        SW_NS_PED = 0;//         Call PED_Control function with direction 0 (NS = upper digit) and 6sec cross time
    }
    Wait_N_Seconds(NS, 9);                                                                          // Step 2: NS LED = GREEN for 9sec
    
    Set_NS(YELLOW);                                                                                 // Step 3: NS LED = YELLOW for 3sec
    Wait_N_Seconds(NS, 3);                                                                       
    
    Set_NS(RED);                                                                                    // Step 4: NS LED = RED
    
    if(EWLT_SW == 1)                                                                                  // Step 5: check if there is a EW left turn request (EWLT_SW ON)
    {
        Set_EWLT(GREEN);                                                                            // Step 6: EWLT LED = GREEN for 6sec 
        Wait_N_Seconds(EWLT, 6);
        
        Set_EWLT(YELLOW);                                                                           // Step 7: EWLT LED = YELLOW for 3sec
        Wait_N_Seconds(EWLT, 3);
        
        Set_EWLT(RED);                                                                              // Step 8: EWLT LED = RED
    }
    
    Set_EW(GREEN);                                                                                  // Step 9: EW LED = GREEN
    
    if(SW_EW_PED == 1)                                                                                 //         check if there is a EW pedestrian (EWPED_SW ON)
    {
        PED_Control(2,9);
        SW_EW_PED = 0;//         Call PED_Control function with direction 1 (EW = lower digit) with 9sec crossing time
    }
        Wait_N_Seconds(EW, 7);                                                                      // Step 10: EW LED = GREEN for 7sec
        
        Set_EW(YELLOW);                                                                             // Step 11: EW LED = YELLOW for 3 sec
        Wait_N_Seconds(EW, 3);
        
        Set_EW(RED);                                                                                // Step 12: EW LED = RED  
    
    if(NSLT_SW == 1)                                                                                  // Step 13: check if there is a NS left turn request (NSLT_SW ON)
    {
        Set_NSLT(GREEN);                                                                            // Step 14: NSLT LED = GREEN for 6sec
        Wait_N_Seconds(NSLT, 6);
        
        Set_NSLT(YELLOW);                                                                           // Step 15: NSLT LED = YELLOW for 3sec
        Wait_N_Seconds(NSLT, 3); 
        
        Set_NSLT(RED);                                                                              // Step 16: NSLT LED = RED
    }
        
    if(FLASHING_REQUEST)
    {
        FLASHING_REQUEST = 0;
        Do_Flashing();
    }
        
}                                                                                                   // end of Day_Mode()

void Night_Mode()                                                                                   // Function for night time logic of traffic light system
{ 
    SW_NS_PED = 0;
    SW_EW_PED = 0;
    MODE = 0;                                                                                       // MODE variable assigned 0, indicating night mode
    MODE_LED = 0;                                                                                   // MODE_LED OFF
    Act_Mode_Txt[0] = 'N';                                                                          // LCD displays 'N' for night mode
    
    // put code here
    Set_NS(GREEN);                                                                                  // Step 1: NS LED = GREEN
    Set_NSLT(RED);                                                                                  //         NSLT LED = RED
    Set_EW(RED);                                                                                    //         EW LED = RED
    Set_EWLT(RED);                                                                                  //         EWLT LED = RED
        
    
    Wait_N_Seconds(NS, 8);                                                                          // Step 2: NS LED = GREEN for 8 seconds 
    
    Set_NS(YELLOW);                                                                                 // Step 3: NS LED = YELLOW for 3sec 
    Wait_N_Seconds(NS, 3);
    
    Set_NS(RED);                                                                                    // Step 4: NS LED = RED
    
    if(EWLT_SW == 1)                                                                                // Step 5: check if EW left turn is requested (EWLT_SW ON) 
    {
        Set_EWLT(GREEN);                                                                            // Step 6: EWLT LED = GREEN for 6sec
        Wait_N_Seconds(EWLT, 6);                                                      
        
        Set_EWLT(YELLOW);                                                                            // Step 7: EWLT LED = YELLOW for 3sec
        Wait_N_Seconds(EWLT, 3);                      
        
        Set_EWLT(RED);                                                                               // Step 8: EWLT LED = RED
    }
    
    Set_EW(GREEN);                                                                                   // Step 9: EW LED = GREEN for 8sec 
    Wait_N_Seconds(EW, 8);
    
    Set_EW(YELLOW);                                                                                  // Step 10: EW LED = YELLOW for 3sec
    Wait_N_Seconds(EW, 3);
    
    Set_EW(RED);                                                                                      // Step 11: EW LED = RED
    
    if (NSLT_SW==1)                                                                                   // Step 12: check if NS left turn is requested (NSLT_SW ON) 
    {
        Set_NSLT(GREEN);                                                                              // Step 13: NSLT LED = GREEN for 6sec
        Wait_N_Seconds(NSLT, 6);                                                      
    
        Set_NSLT(YELLOW);                                                                             // Step 14: NSLT LED = YELLOW for 3sec
        Wait_N_Seconds(NSLT, 3);
    
        Set_NSLT(RED);                                                                                // Step 15: NSLT LED = RED    
    }
}                                                                                                     // end Night_Mode()  


void Old_Wait_One_Second()                                                                            //creates one second delay and blinking asterisk
{
    SEC_LED = 1;                                                                                      // SEC_LED ON
    Wait_Half_Second();                                                                               // Wait for half second (or 500 msec)
    SEC_LED = 0;                                                                                      // SEC_LED OFF
    Wait_Half_Second();                                                                               // Wait for half second (or 500 msec)

}

void Wait_One_Second()                                                                                // creates one second delay and blinking asterisk
{
    SEC_LED = 1;                                                                                      // SEC_LED ON
    strcpy(txt,"*");                                                                                  // Write '*'
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                              // At location (120, 10), Text Color = WHITE, background = BLACK, Text Size = 1
    Wait_Half_Second();                                                                               // Wait for half second (or 500 msec), 

    SEC_LED = 0;                                                                                      // SEC_LED OFF
    strcpy(txt," ");                                                                                  // Write ' ' (get rid of *)
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                              // At location (120,10), Text Color = WHITE, background = BLACK, Text Size = 1
    Wait_Half_Second();                                                                               // Wait for half second (or 500 msec)
    update_LCD_misc();                                                                                // upadte the LCD
}

void Wait_One_Second_With_Beep()                                                                      // creates one second delay as well as sound buzzer
{
    // put code here
 SEC_LED = 1;                                                                                         // First, turn on the SEC LED
 strcpy(txt,"*");                                                                                     // Write '*'                                            
 drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                                 // At location (120, 10), Text Color = WHITE, background = BLACK, Text Size = 1
 Activate_Buzzer();                                                                                   // Activate the buzzer
 Wait_Half_Second();                                                                                  // Wait for half second (or 500 msec)
 
 SEC_LED = 0;                                                                                         // then turn off the SEC LED
 strcpy(txt," ");                                                                                     // Write ' ' (get rid of *)
 drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                                 // At location (120, 10), Text Color = WHITE, background = BLACK, Text Size = 1
 Deactivate_Buzzer ();                                                                                // Deactivate the buzzer
 Wait_Half_Second();                                                                                  // Wait for half second (or 500 msec)
 update_LCD_misc();                                                                                   // update LCD
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                                                                                     // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                                                     // set the lower byte of TMR
    TMR0H = 0x0B;                                                                                     // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                                            // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                                             // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                                                   // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                                             // turn off the Timer 0
}

void Wait_N_Seconds (char direction, char seconds)                                                    // Function for waiting a variable amount of time
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        update_LCD_count(direction, I);                                                               // updates LCD counter
        Wait_One_Second();                                                                            // calls Wait_One_Second for x number of times 
    }
    update_LCD_count(direction, 0);   
}

void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = NS_Cir_Y + direction * 30;    
    
    if (color == Color_Off)                                                                           //if Color off make all circles black but leave outline
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)                                                                           //if the color is red only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    // put code here
    if (color == Color_Green)                                                                         //if the color is green only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
    
    if (color == Color_Yellow)                                                                        //if the color is yellow only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
}

void update_LCD_count(char direction, char count)                                                     // Function for updating the LCD counter
{
   switch (direction)                       
   {
      case NS:                                                                                        // case for the NS direction
        NS_Count[0] = count/10  + '0';                                                                // upper digit
        NS_Count[1] = count%10  + '0';                                                                // lower digit
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);                           // display count
        break;
      
    // put code here
      case NSLT:                                                                                      // case for the NSLT direction
        NSLT_Count[0] = count/10  + '0';                                                              // upper digit
        NSLT_Count[1] = count%10  + '0';                                                              // lower digit
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);                     // display count
        break;
      
      case EW:                                                                                        // case for the EW direction
        EW_Count[0] = count/10  + '0';                                                                // upper digit
        EW_Count[1] = count%10  + '0';                                                                // lower digit
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                           // display count
        break;
        
      case EWLT:                                                                                      // case for the EWLT direction
        EWLT_Count[0] = count/10  + '0';                                                              // upper digit
        EWLT_Count[1] = count%10  + '0';                                                              // lower digit
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);                     // display count
        break;
    }  
}



void update_LCD_misc()
{
    int nStep = get_full_ADC();                                                                       // number of steps after A/D conversion
    volt = nStep * 5 /1024.0;                                                                         // # of volts from # of steps
    SW_MODE = volt < 3 ? 1:0;                                                                       // Day/Night mode logic: 1 = Day, 0 = Night
    
    SW_NSPED = SW_NS_PED;                                                                              // Assign variable to sw input
    SW_NSLT = NSLT_SW;                                                                                // Assign variable to sw input
    SW_EWPED = SW_EW_PED;                                                                              // Assign variable to sw input
    SW_EWLT = EWLT_SW;                                                                                // Assign variable to sw input
      
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0';           else SW_NSPED_Txt[0] = '1';                             // Set Text at bottom of screen to switch states
    // put code here  
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0';             else SW_NSLT_Txt[0] = '1';                              // Set Text at bottom of screen to switch states
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0';           else SW_EWPED_Txt[0] = '1';                             // Set Text at bottom of screen to switch states
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0';             else SW_EWLT_Txt[0] = '1';                              // Set Text at bottom of screen to switch states          
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N';             else SW_MODE_Txt[0] = 'D';                              // Set Text at bottom of screen to switch states
    if (FLASHING_REQUEST == 0) FlashingR_Txt[0] = '0';  else FlashingR_Txt[0] = '1';
    if (FLASHING == 0) FlashingS_Txt[0] = '0';          else FlashingS_Txt[0] = '1';
    
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(70,10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
    drawtext(100,10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);     
    drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                     // Show switch and sensor states at bottom of the screen
    drawtext(32,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(87,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
            
}

void main(void)
{
    Do_Init();                                                                                      // Call init_IO to initialize TRIS registers
    Init_ADC();                                                                                       // Call function to initialize digital and analog I/O
    init_UART();                                                                                      // Call UART code
    OSCCON = 0x70;                                                                                    // set the system clock to be 1MHz 1/4 of the 4MHz
    Initialize_Screen();                                                                              // Initialize the TFT screen


    int nStep = get_full_ADC();                                                                       // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                                         // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    SW_MODE = volt < 2.5 ? 1:0;                                                                       // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                                                                          // forever loop
    {
        if (SW_MODE)                                                                                  // If SW_MODE = 1 = Day Mode
        {
            Day_Mode();                                                                               // calls Day_Mode() function
        }
        else
        {
            SW_NS_PED = 0;
            SW_EW_PED = 0;
            Night_Mode();                                                                             // calls Night_Mode() function
        }
     
    } 
}




