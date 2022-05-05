#include <stdio.h>

#include <p18f4620.h>
#include "I2C_Support.h"
#include "I2C_Soft.h"

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK     1
#define NAK     0


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


void DS1621_Init()
{
    #define Device 0x48
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);                 
}

int DS1621_Read_Temp()
{
    #define Device 0x48                                                                                         // define I2C address for the DS1621 device
    #define Cmd READ_TEMP                                                                                       // define the command for read temperature (0xAA) 
    char Data_Ret;    
    I2C_Start();                                                                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                                                               // Device address
    I2C_Write(Cmd);                                                                                             // Send register address
    I2C_ReStart();                                                                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                                                                               // Initialize data read
    Data_Ret = I2C_Read(NAK);                                                                                   // Data_ret gets the value of whole signal
    I2C_Stop();                                                                                                 // stop I2C
    return Data_Ret;
}

void DS3231_Read_Time()
{
    #define Device 0x68                                                                                         // define I2C address for the DS1621 device
    #define Address 0x00                                                                                        // define Address with the value of second register
    //char Data_Ret;    
    I2C_Start();                                                                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                                                               // DS3231 address Write mode
    I2C_Write(Address);                                                                                         // Send register address
    I2C_ReStart();                                                                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                                                                               // Initialize data read
    second = I2C_Read(ACK);                                                                                     // read the seconds value
    minute = I2C_Read(ACK);                                                                                     // read the minutes value
    hour = I2C_Read(ACK);                                                                                       // read the hours value
    dow = I2C_Read(ACK);                                                                                        // read the day of week value
    day = I2C_Read(ACK);                                                                                        // read the day value 
    month = I2C_Read(ACK);                                                                                      // read the month value
    year = I2C_Read(NAK);                                                                                       // read the year value
    I2C_Stop();    
}

void DS3231_Setup_Time()
{
    #define Device 0x68                                                                                         // define I2C address for the DS1621 device
    #define Address 0x00                                                                                        // define Address with the value of second register
    #define second 0x00                                                                                         // define the seconds value
    #define minute 0x23                                                                                         // define the seconds value
    #define hour 0x09                                                                                           // define the seconds value
    #define dow 0x02                                                                                            // define the seconds value
    #define day 0x21                                                                                            // define the seconds value
    #define month 0x04                                                                                          // define the seconds value
    #define year 0x22                                                                                           // define the seconds value
    I2C_Start();                                                                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                                                               // Device address Write mode
    I2C_Write(Address);                                                                                         // Send register address
    I2C_Write(second);                                                                                          // write seconds value
    I2C_Write(minute);                                                                                          // write minute value
    I2C_Write(hour);                                                                                            // write hour value
    I2C_Write(dow);                                                                                             // write day of week value
    I2C_Write(day);                                                                                             // write day value
    I2C_Write(month);                                                                                           // write month value
    I2C_Write(year);                                                                                            // write year value;             
    I2C_Stop();                                                
}


