/*===================================CPEG222=================================
* Program: Project 3
* Authors: Jonathan Barnes, Joshua Chevalier
* Team #:36
* Date: 10/25/17
* Description: use switches to set a binary number. display in hex on lcd, use for timer up with
 * btnU and timer down with btnD
=============================================================================*/
#include <xc.h>
#include <plib.h>

#include"pmods.h"
#include"config.h"
#include"led.h"
#include "utils.h"
#include "lcd.h"
#include <stdio.h>
#include "ssd.h"
#include<math.h>
#include "srv.h"
#include"rgbled.h"
#include"i2c.h"
#include"acl.h"
#include"mic.h"
#include"aic.h"
#include"adc.h"
#pragma config FNOSC = PRIPLL 
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_1 
#pragma config FPBDIV = DIV_2

char strMsg1[85];
 char strMsg[80];
 int d1,d2,d3,d4 = 0;   
 int count = 0;
 int isStop = 1;
 int d = 0;  // 0: stop  1: forward  2: right  3: left  4: reverse
 int prev;
 int FR = 0;
 int MR = 0;
 int ML = 0;
 int FL = 0;
 float x, y, z = 0;
 int clap = 0;
 int amp = 0;
 int isTest = 1;
 int stopCount = 0;
 int isStarted = 0;
 int TD =0;
 int SpinDone = 0;
    #define FOSC 80000000
    #define CORE_TICK_PERIOD (FOSC / 2 / 10)
void delay_ms(int ms){
    int i,counter;
    for (counter=0; counter< ms; counter ++){
        for(i =0;i<70;i++){}   //software delay 1
       }
}


int main(void) {
    DDPCONbits.JTAGEN = 0;
    LCD_Init();
    SSD_Init();
    SSD_Timer1Setup();
    SRV_Init();
    LED_Init();
    ACL_Init();
    RGBLED_Init();
     MIC_Init();
    
    PMODS_InitPin(1, 1, 1, 0, 0);
    PMODS_InitPin(1, 2, 1, 0, 0);
    PMODS_InitPin(1, 3, 1, 0, 0);
    PMODS_InitPin(1, 4, 1, 0, 0);
    
    #define btnc PORTFbits.RF0
    #define btnu PORTBbits.RB1
    PORTSetPinsDigitalIn(IOPORT_B, BIT_1);
    PORTSetPinsDigitalIn(IOPORT_F, BIT_0);
    btnc = btnu = 0;
    
    
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    OpenCoreTimer(CORE_TICK_PERIOD);  
    mConfigIntCoreTimer(CT_INT_ON | CT_INT_PRIOR_2); 
    INTEnableSystemMultiVectoredInt();
    
    while(1)
    {
        //mabe move direc back into if statement also mabe move sensors
        Direc();`
        sprintf(strMsg1, "Team:36 ");
        LCD_WriteStringAtPos(strMsg1,0,0);
        amp = MIC_Val();
        
        int x = abs(x);
        int y = abs(y);
        int z = abs(z);
       //mabe turn off led to save power for a bit?
        if(isStop == 1)
            {
                RGBLED_SetValue(100, 0, 0);
            }
        else 
            {
            RGBLED_SetValue(0, x * 100, y* 100);
            }
        
        if(isTest == 0 && isStop == 1 && isStarted ==0)
        {
             sprintf(strMsg, "Ready   ");
            LCD_WriteStringAtPos(strMsg,1,0);
            
            if(isStop == 1 && clap >= 2)
            {                       
                isStop = 0;
                clap = 0;
                isStarted = 1;
            }
            if(amp > 550 && isStop == 1)
            {
                clap = clap +1;
            }
        }
        else if(isTest == 0)
        {
            sprintf(strMsg, "Ready   ");
            LCD_WriteStringAtPos(strMsg,1,0);
            
            //mabe add some delays in
            if(d == 0)
            {
                Stop();
                stopCount = stopCount + 1;
            }
            else if(d == 1)
            {
                Forward();
                stopCount = 0;
            }
            else if(d == 2)
            { 
                Right();
                stopCount = 0;
            }
            else if(d == 3)
            {  
                Left();
                stopCount = 0;
            }
            else if(d == 4)
            {
                Reverse();
                stopCount = 0;
            }
        
            if(stopCount >= 10)
            {
                isStop = 1;
                
                TD = 1;
            }
            
        
        }
        else
        {
             sprintf(strMsg, "Test   ");
            LCD_WriteStringAtPos(strMsg,1,0);
            if(amp >= 1022)
            {
            LED_SetGroupValue(0b11111111);
            }
            else if(amp >= 948 )
            {
            LED_SetGroupValue(0b01111111);
            }
            else if(amp >= 873)
            {
            LED_SetGroupValue(0b00111111);
            }
            else if(amp >= 798)
            {
             LED_SetGroupValue(0b00011111);
            }
            else if(amp >= 724)
            {
            LED_SetGroupValue(0b00001111);
            }
            else if(amp >= 649)
            {
            LED_SetGroupValue(0b00000111);
            }
            else if(amp >= 574)
            {
            LED_SetGroupValue(0b00000011);
            }
            else if(amp <= 500)
            {
            LED_SetGroupValue(0b000000001);
            }
        }
        
        // celebration
        if(TD == 1)
        {
            delay_ms(4000);
            if(SpinDone == 0)
            {
                int DoABaralRoll = 0;
                for(DoABaralRoll; DoABaralRoll < 10000; DoABaralRoll++)
                {
                    Spin();
                }
                SpinDone = 1;
            }
            else
            {
                Stop();
                LED_SetGroupValue(0b11111111);
            }
            
        }
        
        //timer
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
    
}

void Direc()
{
    if(FR == 1 && MR == 1 && ML == 1 && FL == 1)
    {
        d = 4;  //good
    }
    else if(FR == 1 && MR == 1 && ML == 1 && FL == 0)
    {
        d = 3;  //good
    }
    else if(FR == 1 && MR == 1 && ML == 0 && FL == 1)
    {
        d = 3; //i think good
    }
    else if(FR == 1 && MR == 1 && ML == 0 && FL == 0)
    {
        d = 1; //i sort of think good (mabe left)
    }
    else if(FR == 1 && MR == 0 && ML == 1 && FL == 1)
    {
        d = 2; // i think good
    }
    else if(FR == 1 && MR == 0 && ML == 1 && FL == 0)
    {
        d = 3; //good
    }
    else if(FR == 1 && MR == 0 && ML == 0 && FL == 1)
    {
        d = 1; //good
    }
    else if(FR == 1 && MR == 0 && ML == 0 && FL == 0)
    {
        d = 3; //good
    }
    else if(FR == 0 && MR == 1 && ML == 1 && FL == 1)
    {
        d = 2; // this may need to be a left  (actualy probably good)
    }
    else if(FR == 0 && MR == 1 && ML == 1 && FL == 0)
    {
        d = 2; //this is just a terible case
    }
    else if(FR == 0 && MR == 1 && ML == 0 && FL == 1)
    {
        d = 2; //good
    }
    else if(FR == 0 && MR == 1 && ML == 0 && FL == 0)
    {
        d = 1; // may need to be right
    }
    else if(FR == 0 && MR == 0 && ML == 1 && FL == 1)
    {
        d = 1; // i sort of think good
    }
    else if(FR == 0 && MR == 0 && ML == 1 && FL == 0)
    {
        d = 3; //good
    }
    else if(FR == 0 && MR == 0 && ML == 0 && FL == 1)
    {
        d = 2; // may need to be a left
    }
    else if(FR == 0 && MR == 0 && ML == 0 && FL == 0)
    {
        d = 0; //good
    }
}

void Forward()
{
    SRV_SetPulseMicroseconds1(1200);
    SRV_SetPulseMicroseconds2(1800);
}
void Right()
{
    SRV_SetPulseMicroseconds1(1500);
    SRV_SetPulseMicroseconds2(1800);
}

void Left()
{
    SRV_SetPulseMicroseconds1(1200);
    SRV_SetPulseMicroseconds2(1450);
}

void Reverse()
{
    SRV_SetPulseMicroseconds1(1800);
    SRV_SetPulseMicroseconds2(1200);
}

void Stop()
{
    SRV_SetPulseMicroseconds1(1500);
    SRV_SetPulseMicroseconds2(1450);
}

void Spin()
{
    SRV_SetPulseMicroseconds1(1200);
    SRV_SetPulseMicroseconds2(1200);
}


void __ISR(_CORE_TIMER_VECTOR, IPL2AUTO) CoreTimerHandler(void) {     // clear the interrupt flag     
    mCTClearIntFlag(); 
    //mabe remove acl
    
    float nums[3];
    ACL_ReadGValues(nums);
     x = nums[0];
     y = nums[1];
     z = nums[2];
     
    
    if(btnu == 1 && isTest == 0)
    {
        isStop = 0;
        isStarted = 1;
    }
    
     if(btnc == 1)
     {
         isTest = 0;
     }
     
    if(isStop == 0 && isTest == 0)
    {
         FR = PMODS_GetValue(1, 1);
         MR = PMODS_GetValue(1, 2);
         ML = PMODS_GetValue(1, 3);
         FL = PMODS_GetValue(1, 4);
         
         count ++;
         
    }
     UpdateCoreTimer(CORE_TICK_PERIOD);
}