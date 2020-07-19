/*===================================CPEG222=================================
* Program: Project 1
* Authors: Jonathan Barnes, Joshua Chevalier
* Team #:36
* Date: 10/12/17
* Description: use switches to set a binary number. display in hex on lcd, use for timer up with
 * btnU and timer down with btnD
=============================================================================*/
#include <xc.h>
#include <plib.h>

#include "btn.h"
#include "utils.h"
#include "lcd.h"
#include <stdio.h>
#include "ssd.h"
#include<math.h>
#include"audio.h"
#include"mic.h"
#include"adc.h"
#pragma config FNOSC = PRIPLL
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_1 
#pragma config FPBDIV = DIV_8
 #define FOSC 80E6 
    #define CORE_TICK_PERIOD (FOSC/25)

int d = 0;
int hex1 = 0;
    int hex2 = 0;
    int totH = 0;
    int d1,d2,d3,d4 = 0;
    char strMsg[80];   
    int count = 0;
    int tot = 0;
    int totI = 0;

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
    LED_Init();
    LCD_Init();
    SSD_Init();
    #define btnl PORTBbits.RB0
    #define btnr PORTBbits.RB8
    #define btnc PORTFbits.RF0
    #define btnu PORTBbits.RB1
    #define btnd PORTAbits.RA15
    #define pot PORTBbits.RB2 
    PORTSetPinsDigitalIn(IOPORT_B, BIT_0|BIT_8|BIT_2|BIT_1);
    PORTSetPinsDigitalIn(IOPORT_F, BIT_0);
    PORTSetPinsDigitalIn(IOPORT_A, BIT_15);
    btnl = btnr = btnc = btnu = btnd = 0;
    
    SSD_Timer1Setup();
    
}


void __ISR(_CORE_TIMER_VECTOR, IPL2AUTO) CoreTimerHandler(void) {     // clear the interrupt flag     
    mCTClearIntFlag(); 
    
    // .. things to do 
    unsigned short buttonLock= 0; 
    // Debouncing variable  lo
    if(btnu && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 1;
        count = 0;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnd && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 2;
        count = totI;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnc && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 0;
        
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(!btnc && !btnu && !btnd && buttonLock)//BTN1 is off and the button is ?locked?
    {
        delay_ms(100); //Wait about 100 for switch to settle 
        buttonLock= 0;// ?unlock? the button
    }
    
    if(d == 1) //left
    {
        if(count < totI)
        {
         count ++;
         d4 = count % 10;
         count = count / 10;
         d3 = count % 10;
         count = count /10;
         d2 = count %10;
         count = count / 10;
         d1 = count % 10;
              
         SSD_WriteDigits(d4, d3, d2, d1, 0, 1, 0, 0); 
         count = d1*1000 + d2*100 + d3*10 + d4;
        }
       
         if(count == totI)
         {
             AUDIO_Init(0);
             SSD_WriteDigits(d4, d3, d2, d1, 0, 1, 0, 0);
             delay_ms(10000);
             AUDIO_Close();
             d = 0;
         }
        
    }
    if(d == 2) //right
    {
       if(count > 0)
        {
         count --;
         d4 = count % 10;
         count = count / 10;
         d3 = count % 10;
         count = count /10;
         d2 = count %10;
         count = count / 10;
         d1 = count % 10;
              
         SSD_WriteDigits(d4, d3, d2, d1, 0, 1, 0, 0); 
         count = d1*1000 + d2*100 + d3*10 + d4;
        }
       
         if(count == 0)
         {
             AUDIO_Init(0);
             SSD_WriteDigits(d4, d3, d2, d1, 0, 1, 0, 0);
             delay_ms(10000);
             AUDIO_Close();
             d = 0;
         }
    }
    if(d == 0)
    {
        count = 0;
        int x = 0;
        int tot = 0;
        unsigned char bVal = SWT_GetGroupValue();
        LED_SetGroupValue(bVal);
        
        for(x; x<=7; x++)
        {
            if(x<=3)
            {
                hex1 = hex1 + SWT_GetValue(x)* pow(2, x);
            }
            else
            {
                hex2 = hex2 + SWT_GetValue(x)* pow(2, x);
            }
            tot = tot + SWT_GetValue(x)* pow(2, x);
        }
        hex2 = hex2/16;
        char strMsg[80];   
        LCD_WriteStringAtPos("Team:36", 0, 0);
        sprintf(strMsg, "Hex 0x: %x%x",hex2, hex1 );
        LCD_WriteStringAtPos(strMsg,1,0);
       
        tot = tot * 10;
        totI = tot;
        d4 = tot % 10;
        tot = tot / 10;
         d3 = tot % 10;
         tot = tot /10;
         d2 = tot %10;
         tot = tot / 10;
         d1 = tot % 10;
              
         SSD_WriteDigits(d4, d3, d2, d1, 0, 1, 0, 0); 
       
        
        hex1 = 0;
        hex2 = 0;
    }
    // update the period     from pot
    
     UpdateCoreTimer(CORE_TICK_PERIOD);
 // .. Toggle the LED    
    mPORTDToggleBits(BIT_1); 
}

