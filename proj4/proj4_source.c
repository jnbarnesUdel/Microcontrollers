/*===================================CPEG222=================================
* Program: Project 4
* Authors: Jonathan Barnes, Joshua Chevalier
* Team #:36
* Date: 11/9/17
* Description: does stuff
=============================================================================*/
#include <xc.h>
#include <plib.h>
#include"led.h"
#include "utils.h"
#include "lcd.h"
#include"spiflash.h"
#include <stdio.h>
#include "ssd.h"
#include<math.h>
#include"rgbled.h"
#include"i2c.h"
#include"acl.h"
#include"adc.h"
#include "fftc.h"// pre-computed coefficients
#include "dsplib_dsp.h"
#include <math.h>
#include"swt.h"
#pragma config FNOSC = PRIPLL 
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_2 
#pragma config FPBDIV = DIV_4

int d = 0;
    
    int i = 0;
    int val[256];
    int num = 0;
    int count = 0;
    int d1;
    int d2;
    int d3;
    int d4;
    int d1, d2, d3, d4 = 0;
    int tot = 0;
    int amp = 0;
    int shift = 0;
    int freq = 0;
    int sense = 2;
    int pos = 0;
    int x = 0;
    int y = 0;
    int z = 0;
    int temp = 0;
    unsigned char buffer[180];
    unsigned char out[180];
    int spot = 0;
    int g = 0;
    
    
   
    #define FOSC 8000000 
    #define CORE_TICK_PERIOD (FOSC)
void delay_ms(int ms){
    int i,counter;
    for (counter=0; counter< ms; counter ++){
        for(i =0;i<70;i++){}   //software delay 1
       }
}


int main(void) {
    DDPCONbits.JTAGEN = 0;
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    OpenCoreTimer(CORE_TICK_PERIOD);  
    mConfigIntCoreTimer(CT_INT_ON | CT_INT_PRIOR_2); 
    INTEnableSystemMultiVectoredInt();
    SWT_Init();
    LCD_Init();
    LED_Init();
    SSD_Init();
    ACL_Init();
    SSD_Timer1Setup();
    SPIFLASH_Init();
    SPIFLASH_WriteEnable();
    RGBLED_Init();
     #define btnl PORTBbits.RB0
    #define btnr PORTBbits.RB8
    #define btnc PORTFbits.RF0
    #define btnu PORTBbits.RB1
    #define btnd PORTAbits.RA15
    #define pot PORTBbits.RB2 
    #define NUMBER_DATA_POINTS 30  //Number of data points to store
    #define SPIFLASH_PROG_ADDR 0x100
    #define SPIFLASH_PROG_SIZE  6*NUMBER_DATA_POINTS*sizeof(char)
    PORTSetPinsDigitalIn(IOPORT_B, BIT_0|BIT_8|BIT_2|BIT_1);
    PORTSetPinsDigitalIn(IOPORT_F, BIT_0);
    PORTSetPinsDigitalIn(IOPORT_A, BIT_15);
    btnl = btnr = btnc = btnu = btnd = 0;
    

    
}


void __ISR(_CORE_TIMER_VECTOR, IPL2AUTO) CoreTimerHandler(void) {     // clear the interrupt flag     
    mCTClearIntFlag(); 
    
    LED_SetGroupValue(SWT_GetGroupValue()); 
    
    float nums[3];
      ACL_ReadGValues(nums);
     float xf = nums[0];
     float yf = nums[1];
     float zf = nums[2];
     int x = xf * 1000;
     int y = yf * 1000;
     int z = zf * 1000;
     
    
     unsigned short buttonLock= 0; 
    // Debouncing variable  lo
    if(btnl && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 1;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnr && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 2;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnc && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 0;
        
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnu && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        
        d = 3;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnd && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 4;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(!btnl &&!btnr && !btnc && !btnu && !btnd && buttonLock)//BTN1 is off and the button is ?locked?
    {
        delay_ms(100); //Wait about 100 for switch to settle 
        buttonLock= 0;// ?unlock? the button
    }
     if(SWT_GetValue(1) == 1)
     {
         
         count = count +1;
         int p =0;
         for(p; p<180;p++)
         {
             unsigned char rgRawVals[6];
            ACL_ReadRawValues(rgRawVals);
            unsigned char a = rgRawVals[0];
            unsigned char b = rgRawVals[1];
            unsigned char c = rgRawVals[2];
            unsigned char d = rgRawVals[3];
            unsigned char e = rgRawVals[4];
            unsigned char f = rgRawVals[5];
             if(g == 0)
             {
                 buffer[p] = a;
                 g++;
             }
             else if(g == 1)
             {
                 buffer[p] = b;
                 g++;
             }
             else if(g == 2)
             {
                 buffer[p] = c;
                 g++;
             }
             else if(g == 3)
             {
                 buffer[p] = d;
                 g++;
             }
             else if(g == 4)
             {
                 buffer[p] = e;
                 g++;
             }
             else if(g == 5)
             {
                 buffer[p] = f;
                 g = 0;
             }
             
        }
         
         if(count == 1)
         {
             
             char strMsg1[85];
             sprintf(strMsg1, "Erasing Flash");
             delay_ms(500);
            sprintf(strMsg1, "Writing Flash ");
            LCD_WriteStringAtPos(strMsg1,0,0);
            
            SPIFLASH_ProgramPage(SPIFLASH_PROG_ADDR, buffer, SPIFLASH_PROG_SIZE);
            delay_ms(500);
           
            sprintf(strMsg1, "Writing Finished ");
            LCD_WriteStringAtPos(strMsg1,0,0);
            
            SPIFLASH_Read(SPIFLASH_PROG_ADDR, buffer, SPIFLASH_PROG_SIZE);
         }
     }
     
     if(SWT_GetValue(2) == 1)
     {
         
         if(d ==0)
     {
         if(pos == 0)
         {
             unsigned char c1[2];
             c1[0] = buffer[spot * 6];
             c1[1] = buffer[spot*6 + 1];
             
             unsigned char c2[2];
             c2[0] = buffer[spot * 6 + 2];
             c2[1] = buffer[spot*6 + 3];
             
             unsigned char c3[2];
             c3[0] = buffer[spot * 6 + 4];
             c3[1] = buffer[spot*6 + 5];
             
             xf = ACL_ConvertRawToValueG(c1);
             yf = ACL_ConvertRawToValueG(c2);
             zf = ACL_ConvertRawToValueG(c3);
             
             x = xf * 1000;
             y = yf * 1000;
             z = zf * 1000;
             
             char strMsg1[85];
            sprintf(strMsg1, "Team:36 set:%d   ", spot+1);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "x %1.3f y %1.3f   ", xf, yf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = z;
            z = abs(z);
            
            d4 = z % 10;
            z = z / 10;
            d3 = z % 10;
            z = z /10;
            d2 = z %10;
            z = z / 10;
            d1 = z % 10;
           
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            z = temp;
         }
         else if(pos == 1)
         {
              unsigned char c1[2];
             c1[0] = buffer[spot * 6];
             c1[1] = buffer[spot*6 + 1];
             
             unsigned char c2[2];
             c2[0] = buffer[spot * 6 + 2];
             c2[1] = buffer[spot*6 + 3];
             
             unsigned char c3[2];
             c3[0] = buffer[spot * 6 + 4];
             c3[1] = buffer[spot*6 + 5];
             
             xf = ACL_ConvertRawToValueG(c1);
             yf = ACL_ConvertRawToValueG(c2);
             zf = ACL_ConvertRawToValueG(c3);
             
             x = xf * 1000;
             y = yf * 1000;
             z = zf * 1000;
            char strMsg1[80];
            sprintf(strMsg1, "Team:36 set:%d   ", spot+1);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "y %1.3f z %1.3f    ", yf, zf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = x;
            x = abs(x);
            d4 = x % 10;
            x = x / 10;
            d3 = x % 10;
            x = x /10;
            d2 = x %10;
            x = x / 10;
            d1 = x % 10;
            
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            x = temp;
         }
         else if(pos == 2)
         {
           unsigned char c1[2];
             c1[0] = buffer[spot * 6];
             c1[1] = buffer[spot*6 + 1];
             
             unsigned char c2[2];
             c2[0] = buffer[spot * 6 + 2];
             c2[1] = buffer[spot*6 + 3];
             
             unsigned char c3[2];
             c3[0] = buffer[spot * 6 + 4];
             c3[1] = buffer[spot*6 + 5];
             
             xf = ACL_ConvertRawToValueG(c1);
             yf = ACL_ConvertRawToValueG(c2);
             zf = ACL_ConvertRawToValueG(c3);
             
             x = xf * 1000;
             y = yf * 1000;
             z = zf * 1000;
             
             char strMsg1[80];
            sprintf(strMsg1, "Team:36 set:%d   ", spot+1);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "z %1.3f x %1.3f   ", zf, xf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = y;
            y = abs(y);
            d4 = y % 10;
            y = y / 10;
            d3 = y % 10;
            y = y /10;
            d2 = y %10;
            y = y / 10;
            d1 = y % 10;
              
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            y = temp;
         }
         
     }
     if(d==2)
     {
         if(pos == 0)
         {
             pos = 2;
         }
         else if(pos == 1)
         {
             pos = 0;
         }
         else if(pos == 2)
         {
             pos = 1;
         }
         d = 0;
     }
     
     if(d==1)
     {
         if(pos == 0)
         {
             pos = 1;
         }
         else if(pos == 1)
         {
             pos = 2;
         }
         else if(pos == 2)
         {
             pos = 0;
         }
         d = 0;
     }
     
     if(d==3)
     {
         if(spot == 29)
         {
             spot = 29;
         }
         else
         {
             spot = spot +1;
         }
         d = 0;
     }
     
     if(d==4)
     {
         if (spot == 0)
         {
             spot = 0;
         }
         else
         {
             spot = spot -1;
         }
         d = 0;
     }
     
     
     
     xf = abs(x);
     yf = abs(y);
     zf = abs(z);
     if(xf > yf && xf> zf )
    {
        RGBLED_SetValue(1000,0,0);
    }
    else if(yf > xf && yf> zf)
    {
        RGBLED_SetValue(0,1000,0);
    }
    else if(zf > xf && zf > yf)
    {
        RGBLED_SetValue(0,0,1000);
    }
     }
     //////////////////////////////// end of sw2
     
     if(SWT_GetValue(1) == 0)
     {
         count = 0;
     }
     
     if(SWT_GetValue(2) ==0)
     {
     if(d ==0)
     {
         if(pos == 0)
         {
             char strMsg1[85];
            sprintf(strMsg1, "Team:36 sens:%dG ", sense);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "x %1.3f y %1.3f   ", xf, yf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = z;
            z = abs(z);
            
            d4 = z % 10;
            z = z / 10;
            d3 = z % 10;
            z = z /10;
            d2 = z %10;
            z = z / 10;
            d1 = z % 10;
           
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            z = temp;
         }
         else if(pos == 1)
         {
            char strMsg1[80];
            sprintf(strMsg1, "Team:36 sens:%dG", sense);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "y %1.3f z %1.3f    ", yf, zf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = x;
            x = abs(x);
            d4 = x % 10;
            x = x / 10;
            d3 = x % 10;
            x = x /10;
            d2 = x %10;
            x = x / 10;
            d1 = x % 10;
            
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            x = temp;
         }
         else if(pos == 2)
         {
             char strMsg1[80];
            sprintf(strMsg1, "Team:36 sens:%dG", sense);
            LCD_WriteStringAtPos(strMsg1,0,0);
            char strMsg[80];
            sprintf(strMsg, "z %1.3f x %1.3f   ", zf, xf );
            LCD_WriteStringAtPos(strMsg,1,0);
            temp = y;
            y = abs(y);
            d4 = y % 10;
            y = y / 10;
            d3 = y % 10;
            y = y /10;
            d2 = y %10;
            y = y / 10;
            d1 = y % 10;
              
            if(temp > 0)
            {
            SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 1);
            }
            else if(temp < 0)
            {
            SSD_WriteDigits(d3, d2, d1, 17, 0, 0, 1, 0);
            }
            y = temp;
         }
         
     }
     if(d==2)
     {
         if(pos == 0)
         {
             pos = 2;
         }
         else if(pos == 1)
         {
             pos = 0;
         }
         else if(pos == 2)
         {
             pos = 1;
         }
         d = 0;
     }
     
     if(d==1)
     {
         if(pos == 0)
         {
             pos = 1;
         }
         else if(pos == 1)
         {
             pos = 2;
         }
         else if(pos == 2)
         {
             pos = 0;
         }
         d = 0;
     }
     
     if(d==3)
     {
         if(sense == 2)
         {
            sense = 4;
            ACL_SetRange(1);
         }
         else if(sense == 4)
         {
             sense = 8;
             ACL_SetRange(2);
         }
         else
         {
             ACL_SetRange(2);
         }
         d = 0;
     }
     
     if(d==4)
     {
         if (sense == 4)
         {
            sense= 2;
            ACL_SetRange(0);
         }
         else if(sense == 8)
         {
             sense = 4;
             ACL_SetRange(1);
         }
         else
         {
             ACL_SetRange(0);
         }
         d = 0;
     }
     
     
     xf = abs(x);
     yf = abs(y);
     zf = abs(z);
     if(xf > yf && xf> zf )
    {
        RGBLED_SetValue(1000,0,0);
    }
    else if(yf > xf && yf> zf)
    {
        RGBLED_SetValue(0,1000,0);
    }
    else if(zf > xf && zf > yf)
    {
        RGBLED_SetValue(0,0,1000);
    }
     }
     UpdateCoreTimer(CORE_TICK_PERIOD);
 // .. Toggle the LED    
    mPORTDToggleBits(BIT_1); 
}
