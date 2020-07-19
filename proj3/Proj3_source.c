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

#include "utils.h"
#include "lcd.h"
#include <stdio.h>
#include "ssd.h"
#include<math.h>
#include"rgbled.h"
#include"mic.h"
#include"adc.h"
#include "fftc.h"// pre-computed coefficients
#include "dsplib_dsp.h"
#include <math.h>
#pragma config FNOSC = PRIPLL 
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_2 
#pragma config FPBDIV = DIV_4
    
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
    int temp = 0;
    int shift = 0;
    int freq = 0;
    
    //stuff for fft
    #define fftc fft16c128 // from fftc.h, for N = 128
    int16c din[128];
    int16c dout[128];
    int16c scratch[128];
    
    //need magnitude of thing returned from fft
    int mag[128];
    int sortMag[128];
  
    #define FOSC 4000000 
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
    LED_Init();
    LCD_Init();
    SSD_Init();
    SSD_Timer1Setup();
    MIC_Init();
    RGBLED_Init();
}


void __ISR(_CORE_TIMER_VECTOR, IPL2AUTO) CoreTimerHandler(void) {     // clear the interrupt flag     
    mCTClearIntFlag(); 
    
    for(count =0; count < 128; count ++)
    {
        val[count] = amp;
        if(count < 64)
        {
           din[count].re = amp;
        }
        else
        {
           din[count].im = amp;
        }
        tot = 0;
        if(count == 127)
        {
            for(i = 0; i < 126; i++)
            {
                if(val[i] > tot);
                {
                    tot = val[i];
                }
            }
        }
    }
  
     if(tot >= 1022)
    {
       LED_SetGroupValue(0b11111111);
    }
    else if(tot >= 948 )
    {
        LED_SetGroupValue(0b01111111);
        
    }
    else if(tot >= 873)
    {
         LED_SetGroupValue(0b00111111);
    }
    else if(tot >= 798)
    {
        LED_SetGroupValue(0b00011111);
    }
    else if(tot >= 724)
    {
        LED_SetGroupValue(0b00001111);
    }
    else if(tot >= 649)
    {
        LED_SetGroupValue(0b00000111);
    }
    else if(tot >= 574)
    {
        LED_SetGroupValue(0b00000011);
    }
    else if(tot <= 500)
    {
        LED_SetGroupValue(0b000000001);
    }
    
    mips_fft16(dout, din, fftc, scratch, 7);    //8 is the log base 2 of 256
    int x = 0;
    for(x; x < 128; x++)
    {
        mag[x] = pow((pow(dout[x].re, 2) + pow(dout[x].im, 2)),.5);
        int y = 0;
        for(y; y< 127; y++)
        {
            if(mag[x] > sortMag[y])
            {
                temp = sortMag[y];
                sortMag[y]=mag[x];
                mag[x] = temp;
            }
        }
    }
    
    freq = sortMag[1] ;
    
    //rgb led
    if(freq < 250)
    {
        RGBLED_SetValue(tot,0,0);
    }
    else if(freq >=250 && freq < 450)
    {
        RGBLED_SetValue(0,tot,0);
    }
    else
    {
        RGBLED_SetValue(0,0,tot);
    }
    
    d4 = freq % 10;
    freq = freq / 10;
    d3 = freq % 10;
    freq = freq /10;
    d2 = freq %10;
    freq = freq / 10;
    d1 = freq % 10;
              
    SSD_WriteDigits(d4, d3, d2, d1, 0, 0, 0, 0);
    
    // reset sortMag[]
    int z = 0;
    for(z; z< 128; z++)
    {
        sortMag[z] = 0;
    }
    
    
    
    
    if(SWT_GetValue(0) == 1)
         {  
            LCD_WriteStringAtPos("Team:36  mic on ", 0, 0);
            amp = MIC_Val();
         }
         else
         {   
            LCD_WriteStringAtPos("Team:36  mic off", 0, 0);
         }   
        char strMsg[80];
        sprintf(strMsg, "Signal: %d ", tot );
        LCD_WriteStringAtPos(strMsg,1,0);
        
         
     UpdateCoreTimer(CORE_TICK_PERIOD);
 // .. Toggle the LED    
    mPORTDToggleBits(BIT_1); 
}



